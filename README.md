# kuanyi
左写好的Django后端需要部署到服务器上，但是会有这样那样的问题，本文记录了纯Nginx部署和Nginx+uwsgi联合部署的两种方法。

部署前的准备

数据库推荐用sqlite的db3数据库，对于较少的数据量的应用，上SQL感觉有点小题大做了（才不是因为SQL经常出这样那样的问题而且还花了好多时间都弄不好呢）。

修改settings.py，取消debug模式，设置静态文件的目录。

DEBUG = False
ALLOWED_HOSTS = ["127.0.0.1"]
STATIC_ROOT = os.path.join(BASE_DIR, 'static')
顺序执行下面的指令，同步数据库，建立管理员账户并收集静态文件。

python manage.py makemigrations
python manage.py migrate
python manage.py createsuperuser
python manage.py collectstatic
处于非debug模式下时，服务器是无法正常加载静态资源的，所以需要运行python manage.py collectstatic将静态资源收集到settings.py文件中设置的STATIC_ROOT目录中。

Nginx部署

Nginx是简单的三层结构（下图右），对于业余玩家而言感觉足够了，加上一些花里胡哨(并不)的中间件感觉还手忙脚乱的。

Image
Nginx所做的工作是将通过域名访问的http请求反向代理到服务器上的静态资源或者服务器上的服务。

Nginx的默认配置路径为/etc/nginx/sites-available/default。sites-available是nginx的配置文件，sites-enabled是指向sites-available实体的软链接。在sites-available新建一个配置文件如下

#demo-django
server {
    listen 80;
    listen [::]:80;

    server_name  aaa.bbbb.ccc;

    location / {
        proxy_pass http://127.0.0.1:8000;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection upgrade;
        proxy_set_header Accept-Encoding gzip;
    }

    location /static {
        alias /home/ubuntu/demo-django/static;
    }
    location /media {
        alias /home/ubuntu/demo-django/media;
    }

}
然后将其软链接至sites-enabled并重新加载nginx

sudo ln ../sites-available/demo-django .
sudo nginx -s reload
此时通过域名aaa.bbbb.ccc访问的请求会被nginx代理到服务器上的8000端口，也就是django服务，从而完成了django在服务器上的部署。

可以在tmux中通过下面的指令运行django，需要指定端口的时候加上--port参数，默认是在8000端口。

python manage.py runserver
python manage.py runserver --port 9008
Nginx+uwsgi部署

Nginx对静态文件的代理是比较好的，抗高并发、轻量级，但是它对动态请求的支持不太好。而uwsgi对动态请求的支持比较好，二者互补。于是就出现了四层的代理结构。

Nginx配置

Nginx的配置如下

server {
    listen         80; 
    server_name    aaa.bbb.ccc; 
    charset UTF-8;

    location / { 
        # 通过uwsgi转发请求
        include uwsgi_params;                   
        uwsgi_pass 127.0.0.1:8000;              
        # 设置请求超时时间
        uwsgi_read_timeout 15;                  
    }   
}
重复之前的刷新操作，Nginx部分配置完成。

uwsgi配置

安装uwsgi及插件

sudo apt install uwsgi uwsgi-plugin-python3
然后在django项目文件中新建uwsgi.ini 如下

# uwsgi.ini file
[uwsgi]
# Django-related settings
socket = :8000
# the base directory (full path)
chdir = /home/project/web

# Django s wsgi file
module = web.wsgi

# process-related settings
master = true

# maximum number of worker processes
#maximum number of worker threadsthreads = 5
processes = 5

# try to remove all of the generated file/sockets
vacuum = true

plugin = python3
然后通过uwsgi启动django项目

cd /home/project/web
uwsgi --ini uwsgi.ini
总结

本文详细记录了使用Nginx部署django项目和Nginx+uwsgi联合部署的例子，方便后续回忆和部署。
