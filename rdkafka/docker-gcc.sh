docker rm -f rdkaf
docker run -it --rm -v /etc/hosts:/etc/hosts -v $(pwd):/home --name rdkaf veiasai/rdkafka:v4 /bin/bash
