
```
        _          _ ______ _____ _____ 
       (_)        | |  ____|  __ \_   _|
  _ __  ___  _____| | |__  | |  | || |  
 | '_ \| \ \/ / _ \ |  __| | |  | || |  
 | |_) | |>  <  __/ | |____| |__| || |_ 
 | .__/|_/_/\_\___|_|______|_____/_____|
 | |                                    
 |_|                                    
                             
```
www.pixeledi.eu | https://linktr.ee/pixeledi
V1.0 | 04.2023



## Docker Installation

https://docs.docker.com/engine/install/ubuntu/


## Docker CLI am Beispiel Focalboard

https://hub.docker.com

```
docker run -d --name focalboard -p 8000:8000 mattermost/focalboard
docker ps
docker stop focalboard
docker start focalboard

docker container ls
docker image ls

//alle gestoppten container entfernen
docker container prune
```
## Portainer

Doku: https://docs.portainer.io/start/install/server/docker/linux

```
docker run -d -p 9000:9000 -p 9443:9443 --name portainer \
--restart=always -v /var/run/docker.sock:/var/run/docker.sock \
-v portainer_data:/data portainer/portainer-ce:latest
```

## Mosquitto MQTT Broker

https://hub.docker.com/_/eclipse-mosquitto/


* mkdir mosquitto
sudo chown -R 1883:1883 mosquitto
* cd mosquitto
* nano mosquitto.conf

```bash
allow_anonymous true
listener 1883 0.0.0.0
persistence true
persistence_location /mosquitto/data/
log_dest file /mosquitto/log/mosquitto.log
```

Firewall Port:  ```ufw allow 1883```

```yaml
docker run -d \
    -p 1883:1883 \
    -p 9001:9001 \
    -v $PWD/mosquitto.conf:/mosquitto/config/mosquitto.conf \
    -v $PWD/mosquitto_data:/mosquitto/data \
    -v $PWD/mosquitto_log:/mosquitto/log \
    -v $PWD/config:/mosquitto/config \
    --restart=always \
    --name=mosquitto-server \
    eclipse-mosquitto:latest
```
## ohmyz

https://ohmyz.sh/#install

## mosquitto mit passwordschutz


https://mosquitto.org/man/mosquitto_passwd-1.html

Config

```bash
allow_anonymous false
password_file /mosquitto/config/mosquitto.passwd
listener 1883 0.0.0.0

persistence true
persistence_location /mosquitto/data/
log_dest file /mosquitto/log/mosquitto.log
```

For password generation we need a running Mosquitto container. The password is copied and then saved in the mosquitto.passwd file. Then the container is loaded WITH this file. 


* docker exec -it mosquitto-server sh
* mosquitto_passwd -b -c passwd pixeledi mysecretpassword
* cat passwd
* copy 
* strg+d exit shell
* Die obige Ausgabe wird nun in das File: "mosquitto.passwd" gespeichert:   
```
pixeledi:$7$101$y1og8rkgTbXKpIAv$E3ZYe3VxzM9PtjTxksIqQIwHq6mQ3INmXdrc/
YK6SFwYXUtJTB3MXb7OFZklCKrZJrBYGLEoZFIdPmszqKN3yw==
```
* kill existing Container
* docker stop mosquitto-server
* docker container prune

We need to add a line in the Docker run to do this 

`-v $PWD/mosquitto.passwd:/mosquitto/config/mosquitto.passwd \`

in  mosquitto_conf we have to change -> allow_anonymous false

New Docker run command
```yaml
docker run -it -d \
    -p 1883:1883 \
    -p 9001:9001 \
    -v $PWD/mosquitto.conf:/mosquitto/config/mosquitto.conf \
    -v $PWD/mosquitto_data:/mosquitto/data \
    -v $PWD/mosquitto_log:/mosquitto/log \
    -v $PWD/mosquitto.passwd:/mosquitto/config/mosquitto.passwd \
    --restart=always \
    --name=mosquitto-server \
    eclipse-mosquitto:latest
```

* docker exec -it mosquitto-server sh
* mosquitto_pub -h localhost -t test -m "test" -u "pixeledi" -P "mysecretpassword"
* lässt man die Parameter -u und -P weg kommt eine Fehlermeldung. Somit haben wir die Benutzerabfrage eingefügt

## Node-RED 
Quelle: https://nodered.org/docs/getting-started/docker

docker volume create nodered_data
mkdir nodered
chown -R 1000:1000 nodered
cd nodered
mkdir nodered_data
chown -R 1000:1000 nodered_data
cd nodered_data

für zusatzbeispiel
chown -R 1000:1000 ocr


```yaml
docker run -d \
    -p 1880:1880 \
    -v $PWD/nodered_data:/data \
    --name=nodered \
    --restart=always \
    nodered/node-red:latest
```

Connection in Node-RED with Mosquitto: http://116.203.190.120 (URL von Server) Port 1883

## Node-RED mit Passwortschutz
* Container must be running
* docker exec -it nodered sh
* npx node-red admin hash-pw
* mysecretpassword 2x 
* copy: $2b$08$DOAyahPESu3Pg6w5O5q0Qurzv74cujz1amgZIs9u2xydMGhFG9RIu
* exit oder strg+d

* cd nodered_data
* nano settings.js
* strg+w and search for adminauth
* delete the "//" comments
* inside the nodered folder: cp nodered_data/settings.js settings.js
* docker stop nodered
* docker container prune

```yaml
 adminAuth: {
        type: "credentials",
        users: [{
            username: "admin",
            password: "$2b$08$DOAyahPESu3Pg6w5O5q0Qurzv74cujz1amgZIs9u2xydMGhFG9RIu",
            permissions: "*"
        }]
    },
```

```yaml
docker run -d \
    -p 1880:1880 \
    -v $PWD/nodered_data:/data \
    -v $PWD/settings.js:/data/settings.js \
    -e "TZ=Europe/Brussels" \
    --name=nodered \
    --restart=always \
    nodered/node-red:latest
``` 


## Influxdb
Quelle: https://hub.docker.com/_/influxdb/

mkdir influxdb
cd influxdb

```yaml
docker volume create influxdb
 
docker run -d \
    -p 8086:8086 \
    -v $PWD/influxdb:/var/lib/influxdb2 \
    --name=influxdb \
    --restart=always \
    influxdb:latest

```

## Grafana
Quelle: https://hub.docker.com/r/grafana/grafana/

First login: admin admin

Settings Datasource:
* Flux
* nicht localhost sondern http://ip:8086
* basic auth off
* organization
* in influxdb neuen API Key erstellen

 https://docs.influxdata.com/influxdb/v2.5/tools/grafana/?t=InfluxQL

```yaml
docker volume create grafana
 
docker run -d \
    -p 3000:3000 \
    --name=grafana \
    --restart=always \
    grafana/grafana:latest
```

## MariaDB

Qulle: https://mariadb.com/resources/blog/get-started-with-mariadb-using-docker-in-3-steps/


```
docker run -d \
    -p 3306:3306  \
    --name mdb \
    -e MARIADB_ROOT_PASSWORD=mysecretpassword \
    -d mariadb:latest
```

### MariaDB CLI im Container
The parameter "-p" is passed directly with the password without spaces.
`docker exec -it mdb mariadb --user root -pmysecretpassword`

`MariaDB [(none)]> CREATE DATABASE noderedkurs;`

## Adminer
Quelle: https://www.adminer.org/

--link to the  --name from MariaDB container
```
docker run -d \
    --link mdb:db \
    -p 8080:8080 \
    --name adminer \
    --restart=always \
    adminer:latest
```

## Docker Container updaten using Node-RED as an example

Since we have created a volume before, the data is preserved and the version of Node-RED can be updated.

```bash
docker pull nodered/node-red
docker stop nodered
docker rm nodered
$ docker run ....
```


## Traefik

***docker-compose.yml***
```yaml
version: '3.4'

services:
  traefik:
    container_name: traefik
    restart: unless-stopped
    image: traefik
    networks:
      - iotnetwork
    ports:
      - "80:80"
      - "443:443"
    volumes:
      - /var/run/docker.sock:/var/run/docker.sock:ro
      - ./traefik.yml:/traefik.yml
      - ./traefik_api.yml:/traefik_api.yml
      - ./acme.json:/cert/acme.json
networks:
  iotnetwork:
    external: true
```

***traefik_api.yml***

* at first we create a password with httpd container
* docker run --rm httpd htpasswd -nb admin meinpasswort 

```yaml
http:
  middlewares:
    simpleAuth:
      basicAuth:
        users:
          - 'admin:$apr1$vLPlXwt8$soFFkjXwFDgL3OlHPJb8G.'
  routers:
    api:
      rule: Host(`traefikapp.duckdns.org`)
      entrypoints:
        - websecure
      middlewares:
        - simpleAuth
      service: api@internal
      tls:
        certResolver: lets-encrypt
```

***traefik.yml***
```yaml
entryPoints:
  iotnetwork:
    address: ':80'
    http:
      redirections:
        entryPoint:
          to: websecure
          scheme: https
  websecure:
    address: ':443'
api:
  dashboard: true
certificatesResolvers:
  lets-encrypt:
    acme:
      email: iotnetwork@gmail.com
      storage: acme.json
      tlsChallenge: {}
providers:
  docker:
    watch: true
    network: iotnetwork
  file:
    filename: traefik_api.yml

```


## Docker-compose mit Node-RED und Traefik labels

```
version: "3.7"

services:
  node-red:
    image: nodered/node-red:latest
    environment:
      - TZ=Europe/Amsterdam
    ports:
      - "1880:1880"
    networks:
      - node-red-net
      - iotnetwork
    volumes:
      - "./nodered_data:/data"
    labels:
      - traefik.enable=true
      - traefik.http.routers.node-red.rule=Host(`noderedubuntu.duckdns.org`)
      - traefik.http.routers.node-red.tls=true
      - traefik.http.routers.node-red.tls.certresolver=lets-encrypt
      - traefik.http.services.node-red.loadbalancer.server.port=1880

volumes:
  nodered-data:

networks:
  node-red-net:
  iotnetwork:
    external: true

```

## crontab
`nano /etc/crontab`

`* * * * * root echo "mein erster Cronjob \n" >> /usr/local/sbin/cron.txt`




