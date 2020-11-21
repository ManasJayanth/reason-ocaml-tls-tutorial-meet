#! /bin/sh

openssl s_client  -connect localhost:4433 -CAfile /path/to/project/root/certs/myserver.crt
