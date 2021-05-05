from easysnmp import Session
import os

# Getting environment variables:
# HOST
# PORT
# AUTH_PASS
# PRIV_PASS

class EnvVars:
    def __init__(self):
        self.host = os.getenv('HOST')
        self.port = os.getenv('PORT')
        self.auth_pass = os.getenv('AUTH_PASS')
        self.priv_pass = os.getenv('PRIV_PASS')

    def get_host(self):
        return self.host

    def get_port(self):
        return self.port

    def get_auth_pass(self):
        return self.auth_pass

    def get_priv_pass(self):
        return self.priv_pass

# examles of using easysnmp modlue https://easysnmp.readthedocs.io/en/latest/

env_vars = EnvVars()
# Create an SNMP session to be used for all our requests
session = Session(hostname = 'snmpd:1662', version = 3, 
                  security_level = 'auth_with_privacy', security_username = 'mynewuser', 
                  privacy_protocol = 'AES', privacy_password = env_vars.get_priv_pass(),
                  auth_protocol = 'SHA', auth_password = env_vars.get_auth_pass())

# You may retrieve an individual OID using an SNMP GET
myIntMibObject = session.get('1.3.6.1.4.1.8073.1.1.4')
print ('{oid} | {oid_index} | {snmp_type}  | {value}'.format(
        oid=myIntMibObject.oid,
        oid_index=myIntMibObject.oid_index,
        snmp_type=myIntMibObject.snmp_type,
        value=myIntMibObject.value
    ))

# You may also specify the OID as a tuple (name, index)
# Note: the index is specified as a string as it can be of other types than
# just a regular integer
'''
contact = session.get(('sysContact', '0'))
print ('{oid}.{oid_index} {snmp_type} = {value}'.format(
        oid=contact.oid,
        oid_index=contact.oid_index,
        snmp_type=contact.snmp_type,
        value=contact.value
    ))
'''
# And of course, you may use the numeric OID too
#description = session.get('.1.3.6.1.2.1.1.1.0')

# Set a variable using an SNMP SET
session.set('1.3.6.1.4.1.8073.1.1.5.0', 'its working just fine', 's')
myStringMibObject = session.get('1.3.6.1.4.1.8073.1.1.5.0')
print ('{oid} | {oid_index} | {snmp_type} | {value}'.format(
        oid=myStringMibObject.oid,
        oid_index=myStringMibObject.oid_index,
        snmp_type=myStringMibObject.snmp_type,
        value=myStringMibObject.value
    ))

# Perform an SNMP walk
#system_items = session.walk('system')

# Each returned item can be used normally as its related type (str or int)
# but also has several extended attributes with SNMP-specific information
'''
for item in system_items:
    print ('{oid}.{oid_index} {snmp_type} = {value}'.format(
        oid=item.oid,
        oid_index=item.oid_index,
        snmp_type=item.snmp_type,
        value=item.value
    ))
'''