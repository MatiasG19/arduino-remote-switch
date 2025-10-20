FROM nginx:1.29.1-alpine
ARG TAG=0.0.1
COPY ./src /usr/share/nginx/html
RUN mv /usr/share/nginx/html/index.htm /usr/share/nginx/html/index.html
RUN sed -i "s/%version%/$TAG/g" /usr/share/nginx/html/index.html
EXPOSE 80