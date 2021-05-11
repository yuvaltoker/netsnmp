import easysnmp
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
        self.user_name = os.getenv('USER_NAME')

    def get_server_host(self):
        return self.server_host

    def get_server_port(self):
        return self.server_port

    def get_auth_pass(self):
        return self.auth_pass

    def get_priv_pass(self):
        return self.priv_pass

    def get_user_name(self):
        return self.user_name


class RequestHandler(threading.Thread):

    def __init__(self, env_vars, msg, sock):
        threading.Thread.__init__(self)
        self.env_vars = env_vars
        self.msg = msg
        self.json_string = ''
        self.sock = sock

    def handle_request(self):
        try:
            request_json = json.loads(self.msg)

            # build the json response for where Exception may accure
            json_dict = { 'method' : request_json['method'],
                          'destination' : request_json['destination'],
                          'oid' : request_json['oid'],
                          'name' : request_json['name'],
                          'type' : 'snmpResponse',
                          'time' : 'timestamp', # will be override
                          'hash' : request_json['hash'],
                          'status' : 'S'}

            timestamp = datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
            json_dict['time'] = timestamp

            if request_json['method'] == 'SET':
                self.snmp_set(request_json['oid'],
                              request_json['value'],
                              request_json['dataType'],
                              request_json['destination'])

            value = self.snmp_get(request_json['oid'],
                                  request_json['destination'])
            if value == 'NOSUCHOBJECT':
                raise Exception('No such reachable mib')
            json_dict['value'] = value
        except Exception as e:
            json_dict['status'] = 'F'
            json_dict['error'] = e.__str__()

        self.json_string = json.dumps(json_dict)


    def snmp_set(self, oid, value, data_type, host_name):
        mib_object = easysnmp.snmp_set(oid, value, data_type, hostname = host_name, version = 3, 
                  security_level = 'auth_with_privacy', security_username = self.env_vars.get_user_name(), 
                  privacy_protocol = 'AES', privacy_password = self.env_vars.get_priv_pass(),
                  auth_protocol = 'SHA', auth_password = self.env_vars.get_auth_pass())

    def snmp_get(self, oid, host_name):
        mib_object = easysnmp.snmp_get(oid, hostname = host_name, version = 3, 
                  security_level = 'auth_with_privacy', security_username = self.env_vars.get_user_name(), 
                  privacy_protocol = 'AES', privacy_password = self.env_vars.get_priv_pass(),
                  auth_protocol = 'SHA', auth_password = self.env_vars.get_auth_pass())
        return mib_object.value

    def send(self):
        print('sent data: %s' % (self.json_string))
        self.sock.send(bytes(self.json_string, 'utf-8'))

    def run(self):
        self.handle_request()
        self.send() 


def main():
    buffer_size = 4096
    env_vars = EnvVars()
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((env_vars.get_server_host(), int(env_vars.get_server_port())))
    while True:
        data = sock.recv(buffer_size)
        data = str(data, 'utf-8')
        data = data.lstrip(' ')
        if data != '' and data[:1] == '{':
            print('recv data: %s' % (data))
            handler = RequestHandler(env_vars, data, sock)
            handler.start()
        

if __name__ == '__main__':
    main()


