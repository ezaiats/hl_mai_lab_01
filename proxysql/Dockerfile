FROM proxysql/proxysql:2.3.2
RUN apt-get update -y --fix-missing
RUN apt-get install -y default-mysql-client
ADD proxysql.cnf /etc/
