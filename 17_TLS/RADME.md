sudo apt update
sudo apt install snapd
sudo snap install snap-store
sudo snap install --classic certbot


## copy files
cd /usr/local/sbin/mosquitto
mkdir certs
cp /etc/letsencrypt/live/url/cert.pem cert.pem
... smae for fullchain.pem and privkey.pem


## mosquitto config

'nano /etc/mosquitto/conf.d/default.conf' 

edit the follow
```
listener 8883 0.0.0.0
certfile /mosquitto/certs/cert.pem
cafile /mosquitto/certs/fullchain.pem
keyfile /mosquitto/certs/privkey.pem

```
Restart the container with new volume

```yaml
docker run -it -d \
    -p 1883:1883 \
    -p 8883:8883 \
    -v $PWD/mosquitto.conf:/mosquitto/config/mosquitto.conf \
    -v $PWD/mosquitto_data:/mosquitto/data \
    -v $PWD/mosquitto_log:/mosquitto/log \
    -v $PWD/mosquitto.passwd:/mosquitto/config/mosquitto.passwd \
    -v $PWD/certs:/mosquitto/certs \
    --restart=always \
    --name=mosquitto-server \
    eclipse-mosquitto:latest
```

Enter Container and check

docker exec -it mosquitto-server sh

mosquitto_sub -h hiddenmqtt.duckdns.org -t withssl -m "ssl is the best" -u "pixeledi" -P "noderedkurs#2023"
