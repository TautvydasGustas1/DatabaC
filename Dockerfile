FROM ubuntu
RUN apt-get update -y && apt-get -y install gcc && apt install build-essential -y --no-install-recommends
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN make
CMD ["./databac", "use_env"]
EXPOSE 8080
ENV LISTEN_PORT=8080 FILE_DIR=./database