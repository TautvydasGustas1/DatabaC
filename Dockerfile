FROM gcc
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN make
CMD ["./databac", "use_env"]
EXPOSE 8080
ENV LISTEN_PORT=8080 FILE_DIR=./database
RUN chmod 777 -r database