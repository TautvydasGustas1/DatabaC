FROM ubuntu
RUN apt-get update -y && apt-get -y install gcc && apt install build-essential -y --no-install-recommends && apt-get install -y wget && apt-get -y install vim
COPY --chown=1001:0 . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN make
RUN chgrp -R 0 /usr/src/myapp && \
    chmod -R g=u /usr/src/myapp
CMD ["./databac", "use_env"]
RUN chmod 777 /usr/src/myapp
RUN chmod -R 777 /usr/src/myapp
RUN chown -R 1001:0 /usr/src/myapp
USER 1001
EXPOSE 8080
ENV LISTEN_PORT=8080 FILE_DIR=./database