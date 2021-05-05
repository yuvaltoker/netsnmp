from easysnmp import Session
from datetime import datetime
import os
import threading
import json
import socket

# in class Client, the netsnmp part is using easysnmp module,
# to learn more, enter the following link https://easysnmp.readthedocs.io/en/latest/

# Getting environment variables:
# SERVER_HOST
# SERVER_PORT
# AUTH_PASS
# PRIV_PASS

class EnvVars:
    def __init__(self):
        self.server_host = os.getenv('SERVER_HOST')
        self.server_port = os.getenv('SERVER_PORT')
        self.auth_pass = os.getenv('AUTH_PASS')
        self.priv_pass = os.getenv('PRIV_PASS')

    def get_server_host(self):
        return self.server_host

    def get_server_port(self):
        return self.server_port

    def get_auth_pass(self):
        return self.auth_pass

    def get_priv_pass(self):
        return self.priv_pass


class Client(threading.Thread):

    def __init__(self, env_vars, msg):
        threading.Thread.__init__(self)
        self.env_vars = env_vars
        self.msg = msg
        self.json_string = ''
        self.session = Session(version = 3, 
                  security_level = 'auth_with_privacy', security_username = 'mynewuser', 
                  privacy_protocol = 'AES', privacy_password = self.env_vars.get_priv_pass(),
                  auth_protocol = 'SHA', auth_password = self.env_vars.get_auth_pass())

    def msg_to_json(self):
        request_json = json.loads(self.msg)
        if request_json['method'] == 'SET':
            self.snmp_set(request_json['oid'],
                          request_json['value'],
                          request_json['dataType'],
                          request_json['destination'])

        value = self.snmp_get(request_json['oid'],
                              request_json['destination'])
        timestamp = datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        self.json_string = ('{ "method": ' + request_json['method'] + '"' +
                            ', "destination: ' + request_json['destination'] + '"' +
                            ', "oid": "' + request_json['oid'] + '"' +
                            ', "name": "' + request_json['name'] + '"' +
                            ', "type": "' + 'snmpResponse' + '"' +
                            ', "time": "' + timestamp + '"' +
                            ', "hash": "' + request_json['hash'] + '"' +
                            ', "value": "' + value + '"}')


    def snmp_set(self, oid, value, data_type, host_name):
        session.set(oid, value, data_type, hostname = host_name)

    def snmp_get(self, host_name):
        mib_object = session.get(oid, hostname = host_name)
        return mib_object.value

    def send(self):
        sock = socket.socket(socket.AF_INET, # Internet
                      socket.SOCK_DGRAM) # UDP
        sock.sendto(bytes(self.json_string, 'utf-8'), (self.env_vars.get_server_host, self.env_vars.get_server_port))

    def run(self):
        self.msg_to_json()
        self.send()
        #creating udp socket and sending the 


def main():
    buffer_size = 4096
    env_vars = EnvVars()
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('sam-server', '3044'))
    while True:
        data = s.recv(buffer_size)
        cli = Client()
        cli.start()
        

if __name__ == '__main__':
    main()


