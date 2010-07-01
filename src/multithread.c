/*****************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 */

/* A multi-threaded example that uses pthreads extensively to fetch
 * X remote files at once */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <curl/curl.h>

#define MAXTHREADS 20

#define TIMEOUT   5
#define INPUTFILE "/Users/okoeroo/dvl/scripts/hacking/gaming_games/working.proxies"
#define OUTPUTFILE "my_tested_proxies.txt"
#define TESTURL "http://www.nikhef.nl/~okoeroo/testfile"
#define TESTDATA "foobar2010"
#define USERAGENT "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.2) Gecko/20100115 Firefox/3.6"

typedef struct buffer_s
{
    char *data;
    size_t size;
    size_t offset;
} buffer_t;


pthread_mutex_t output_mutex;



static void *myrealloc(void *ptr, size_t size);

static void *myrealloc(void *ptr, size_t size)
{   
    /* There might be a realloc() out there that doesn't like reallocing
       NULL pointers, so we take care of it here */
    if(ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}   


static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    buffer_t *mem = (buffer_t*)data;

    mem->data= myrealloc(mem->data, mem->size + realsize + 1);
    if (mem->data) {
        memcpy(&(mem->data[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->data[mem->size] = 0;
    }
    return realsize;
}


static void *pull_one_url(void *proxy)
{
    CURL *curl;
    buffer_t chunk;
    FILE * f = NULL;
    sigset_t sigset;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    chunk.data = NULL; /* we expect realloc(NULL, size) to work */ 
    chunk.size = 0;    /* no data at this point */ 

    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, TESTURL);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USERAGENT);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);

    if (proxy)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy);
        curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1L); 
        /* curl_easy_setopt(easyhandle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);  */
    }

    curl_easy_perform(curl); /* ignores error */
    curl_easy_cleanup(curl);

    /* Test the returned information */
    if (chunk.data && (strncmp (chunk.data, TESTDATA, strlen(TESTDATA)) == 0))
    {
        pthread_mutex_lock (&output_mutex);
        if ((f = fopen (OUTPUTFILE, "a")))
        {
            fprintf (f, "%s\n", (char *)proxy);
            fclose(f);
        }
        pthread_mutex_unlock (&output_mutex);
    }

    return NULL;
}


char * getnextproxy (const char * buffer, char * prev_proxy)
{
    char * foo = NULL;
    char * bar = NULL;
    char * ret = NULL;
    int c = 0;

    if (prev_proxy)
    {
        foo = strstr(buffer, prev_proxy);
        foo = &foo[strlen(prev_proxy) + 1];
    }
    else
        foo = buffer;

    if (bar = strchr (foo, '\n'))
    {
        c = strlen(foo) - strlen(bar);
        ret = malloc (sizeof(char) * (c + 1));
        strncpy (ret, foo, c);

        return ret;
    }
    else
        return NULL;


}

int main(int argc, char **argv)
{
    pthread_t tid[MAXTHREADS*100];
    int i = 0;
    int error;
    char * buf = NULL;
    int bufsize = 0;
    char * proxy = NULL;
    FILE * in = NULL;
    int active_threads = 0;


    printf ("Settings:\n");
    printf ("\tthreads:      %d\n", MAXTHREADS);
    printf ("\ttest URL:     %s\n", TESTURL);
    printf ("\tUser Agent:   %s\n", USERAGENT);
    printf ("\tInput file:   %s\n", INPUTFILE);
    printf ("\tOutput file:  %s\n", OUTPUTFILE);
    printf ("\n");

    pthread_mutex_init (&output_mutex, NULL);

    if (in = fopen (INPUTFILE, "r"))
    {
        fseek(in, 0, SEEK_END);
        bufsize = ftell(in) + 1;
        buf = malloc (sizeof(char) * bufsize);
        fseek(in, 0, SEEK_SET);

        buf[fread(buf, 1, bufsize, in)] = '\0';
        fclose (in);

        /* Must initialize libcurl before any threads are started */
        curl_global_init(CURL_GLOBAL_ALL);

        /* ugly haxx */
        while (proxy = getnextproxy(buf, proxy))
        {
            error = pthread_create(&tid[i],
                    NULL, /* default attributes please */
                    pull_one_url,
                    (void *)proxy);
            if(0 != error)
                fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
            else
            {
                fprintf(stderr, "Thread %d, gets via %s\n", i, proxy);
                active_threads++;
                i++;
            }

            if (active_threads < MAXTHREADS)
                continue;
            else
            {
                active_threads = 0;
                /* now wait for all threads to terminate */
                for(i=0; i< MAXTHREADS; i++) 
                {
                    error = pthread_join(tid[i], NULL);
                    fprintf(stderr, "Thread %d terminated\n", i);
                }
            }
        }
    }

    return 0;
}
