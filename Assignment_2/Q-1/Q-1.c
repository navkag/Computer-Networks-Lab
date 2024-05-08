#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

char dns_servers[10][100];
int dns_server_count = 0;

#define T_A 1
#define T_NS 2
#define T_CNAME 5
#define T_SOA 6
#define T_PTR 12
#define T_MX 15

void ngethostbyname(unsigned char *, int);
void ChangetoDnsNameFormat(unsigned char *, char *);
unsigned char *ReadName(unsigned char *, unsigned char *, int *);
// void get_dns_servers();

struct DNS_HEADER
{
    unsigned short id;

    unsigned char rd : 1;
    unsigned char tc : 1;
    unsigned char aa : 1;
    unsigned char opcode : 4;
    unsigned char qr : 1;

    unsigned char rcode : 4;
    unsigned char cd : 1;
    unsigned char ad : 1;
    unsigned char z : 1;
    unsigned char ra : 1;

    unsigned short q_count;
    unsigned short ans_count;
    unsigned short auth_count;
    unsigned short add_count;
};

struct QUESTION
{
    unsigned short qtype;
    unsigned short qclass;
};

#pragma pack(push, 1)
struct R_DATA
{
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD
{
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};

typedef struct
{
    unsigned char *name;
    struct QUESTION *ques;
} QUERY;

#define MAX_CACHE_SIZE 3

struct WebPage
{
    char url[256];
    char content[1024];
};

struct WebCache
{
    struct WebPage cacheList[MAX_CACHE_SIZE];
    int cacheSize;
};

void initializeWebCache(struct WebCache *cache)
{
    cache->cacheSize = 0;
    memset(cache->cacheList, 0, sizeof(cache->cacheList));
}

void ChangetoDnsNameFormat(unsigned char *dns, char *host)
{
    int lock = 0, i;
    strcat((char *)host, ".");

    for (i = 0; i < strlen((char *)host); i++)
    {
        if (host[i] == '.')
        {
            *dns++ = i - lock;
            for (; lock < i; lock++)
            {
                *dns++ = host[lock];
            }
            lock++;
        }
    }
    *dns++ = '\0';
}

unsigned char *ReadName(unsigned char *reader, unsigned char *buffer, int *count)
{
    unsigned char *name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char *)malloc(256);

    name[0] = '\0';

    // read the names in 3www6google3com format
    while (*reader != 0)
    {
        if (*reader >= 192)
        {
            offset = (*reader) * 256 + *(reader + 1) - 49152; // 49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; // we have jumped to another location so counting wont go up!
        }
        else
        {
            name[p++] = *reader;
        }

        reader = reader + 1;

        if (jumped == 0)
        {
            *count = *count + 1; // if we havent jumped to another location then we can count up
        }
    }

    name[p] = '\0'; // string complete
    if (jumped == 1)
    {
        *count = *count + 1; // number of steps we actually moved forward in the packet
    }

    // now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int)strlen((const char *)name); i++)
    {
        p = name[i];
        for (j = 0; j < (int)p; j++)
        {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0'; // remove the last dot
    return name;
}

char *fetchWebPage(char *host, int query_type)
{
    unsigned char buf[65536], *qname, *reader;
    int i, j, stop, s;

    struct sockaddr_in a;

    struct RES_RECORD answers[20], auth[20], addit[20];
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

    // printf("Resolving %s", host);

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    // dest.sin_addr.s_addr = inet_addr("172.17.1.1");
    dest.sin_addr.s_addr = inet_addr("8.8.8.8");

    dns = (struct DNS_HEADER *)&buf;  // Char treated as dns structure.

    dns->id = (unsigned short)htons(getpid());  // identification (16 bits)
    dns->qr = 0;                                // Query(0) or response(1)
    dns->opcode = 0;                            // Kind of query (std(0) or inverse(1) or server status request(2)) (4 bit)
    dns->aa = 0;                                // when it is 1, responding server is authoritative      
    dns->tc = 0;                                // when it is 1, truncated response 
    dns->rd = 1;                                // when 1, recursive desired
    dns->ra = 0;                                // when 1, recrusive available
    dns->z = 0;                                 // when 0, reserved for future use.
    dns->ad = 0;                                
    dns->cd = 0;
    dns->rcode = 0;                              // 0 - No error, 1 - format error, 2 - server failure, 3 - name error, 4 - not implement, 5 - rejected (4 bit)
    dns->q_count = htons(1);                    // No of questions
    dns->ans_count = 0;                         // No of answers
    dns->auth_count = 0;                        // No of authoritative RRs
    dns->add_count = 0;                          // No of additional RRs

    qname = (unsigned char *)&buf[sizeof(struct DNS_HEADER)];

    ChangetoDnsNameFormat(qname, host);
    qinfo = (struct QUESTION *)&buf[sizeof(struct DNS_HEADER) + (strlen((const char *)qname) + 1)];

    qinfo->qtype = htons(query_type);
    qinfo->qclass = htons(1);

    // printf("\nSending Packet...");
    if (sendto(s, (char *)buf, sizeof(struct DNS_HEADER) + (strlen((const char *)qname) + 1) + sizeof(struct QUESTION), 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
    {
        perror("sendto failed");
    }
    // printf("Done");

    i = sizeof dest;
    // printf("\nReceiving answer...");
    if (recvfrom(s, (char *)buf, 65536, 0, (struct sockaddr *)&dest, (socklen_t *)&i) < 0)
    {
        perror("recvfrom failed");
    }
    // printf("Done");

    dns = (struct DNS_HEADER *)buf;

        if (dns->rcode){
        printf("Invalid hostname!\n");
        exit(0);
    }
    // Extract answers from buffer.
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char *)qname) + 1) + sizeof(struct QUESTION)];

    // printf("\nThe response contains : ");
    // printf("\n %d Questions.", ntohs(dns->q_count));
    // printf("\n %d Answers.", ntohs(dns->ans_count));

    stop = 0;

    for (i = 0; i < ntohs(dns->ans_count); i++)
    {
        answers[i].name = ReadName(reader, buf, &stop);
        reader = reader + stop;

        answers[i].resource = (struct R_DATA *)(reader);
        reader = reader + sizeof(struct R_DATA);

        if (ntohs(answers[i].resource->type) == 1) // if its an ipv4 address
        {
            answers[i].rdata = (unsigned char *)malloc(ntohs(answers[i].resource->data_len));

            for (j = 0; j < ntohs(answers[i].resource->data_len); j++)
            {
                answers[i].rdata[j] = reader[j];
            }

            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';

            reader = reader + ntohs(answers[i].resource->data_len);
        }
        else
        {
            answers[i].rdata = ReadName(reader, buf, &stop);
            reader = reader + stop;
        }
    }

    // printf("\nAnswer Records : %d \n", ntohs(dns->ans_count));
    char *ip_addresses = NULL;
    for (i = 0; i < ntohs(dns->ans_count); i++)
    {
        // printf("Name : %s ", answers[i].name);

        if (ntohs(answers[i].resource->type) == T_A) // IPv4 address
        {
            long *p;
            p = (long *)answers[i].rdata;
            a.sin_addr.s_addr = (*p); // working without ntohl
            char *ip = inet_ntoa(a.sin_addr);
            if (ip_addresses == NULL)
            {
                ip_addresses = strdup(ip);
            }
            else
            {
                char *temp = (char *)malloc(strlen(ip_addresses) + strlen(ip) + 2);
                sprintf(temp, "%s, %s", ip_addresses, ip);
                free(ip_addresses);
                ip_addresses = temp;
            }
        }

        // if (ntohs(answers[i].resource->type) == 5)
        // {
        //     // Canonical name for an alias
        //     printf("has alias name : %s", answers[i].rdata);
        // }

        // printf("\n");
    }
    return ip_addresses;
}

char *getWebPage(struct WebCache *cache, char *url)
{
    int i;
    for (i = 0; i < cache->cacheSize; i++)
    {
        if (strcmp(cache->cacheList[i].url, url) == 0)
        {
            // Page found in cache, move it to the front (most recently used)
            struct WebPage temp = cache->cacheList[i];
            int j;
            for (j = i; j > 0; j--)
            {
                cache->cacheList[j] = cache->cacheList[j - 1];
            }
            cache->cacheList[0] = temp;
            return cache->cacheList[0].content;
        }
    }

    // Page not found in cache, fetch it using HTTP GET
    char *content = fetchWebPage(url, T_A);

    // Check if cache is full
    if (cache->cacheSize >= MAX_CACHE_SIZE)
    {
        // Evict the least recently used page
        cache->cacheSize--;
    }

    // Add the new page to the cache
    struct WebPage newPage;
    int l = strlen(url);
    char *temp;
    strcpy(temp, url);
    temp[l - 1] = '\0';
    printf("%s\n", temp);
    strcpy(newPage.url, temp);
    strcpy(newPage.content, content);

    int j;
    for (j = cache->cacheSize; j > 0; j--)
    {
        cache->cacheList[j] = cache->cacheList[j - 1];
    }
    cache->cacheList[0] = newPage;
    cache->cacheSize++;

    return cache->cacheList[0].content;
}

void displayCache(struct WebCache *cache)
{
    int i;
    for (i = 0; i < cache->cacheSize; i++)
    {
        printf("%d.%s - %s\n", i + 1, cache->cacheList[i].url, cache->cacheList[i].content);
    }
}

int main()
{
    struct WebCache cache;
    initializeWebCache(&cache);

    printf("Cache size: %d\n", MAX_CACHE_SIZE);

    while (1)
    {
        char url[256];
        printf("Enter URL (or 'exit' to quit): ");
        scanf("%s", url);

        if (strcmp(url, "exit") == 0)
        {
            break;
        }

        clock_t checkpoint1 = clock();

        printf("\033[32mIP address(es): \033[0m\n%s\n", getWebPage(&cache, url));

        clock_t checkpoint2 = clock();
        printf("\033[31mTime taken for IP address(es) to load: \033[0m%ld clock cycles.\n", (checkpoint2 - checkpoint1));

        printf("\n\033[33mCache contents: \033[0m\n");
        displayCache(&cache);
        printf("\n");
    }

    printf("Successfully exited!\n");
    return 0;
}
