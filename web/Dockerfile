FROM nginx:1.29.1-alpine
COPY ./web/src /usr/share/nginx/html
RUN mv /usr/share/nginx/html/index.htm /usr/share/nginx/html/index.html
EXPOSE 80