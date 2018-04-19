# ipk-lookup
## IPK - project 2
Basic tool for translating hostnames to IP addresses using DNS

### Usage:
    ./ipk-lookup -s server [-T timeout] [-t type] [-i] name
    
    -s    server to send DNS queries to (IPv4 address)
    -T    timeout for the lookup
    -t    type of the DNS record we are looking for (A, AAA, PTR, NS, CNAME)
    -i    force iterative resolution
    name  domain name to be translated, or IPv4/IPv6 address when -t is PTR
