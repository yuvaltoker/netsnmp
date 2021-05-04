from easysnmp import Session

# Create an SNMP session to be used for all our requests
session = Session(hostname='snmpd:1662', community='public', version=2)

# You may retrieve an individual OID using an SNMP GET
location = session.get('sysLocation.0')

# You may also specify the OID as a tuple (name, index)
# Note: the index is specified as a string as it can be of other types than
# just a regular integer
contact = session.get(('sysContact', '0'))

# And of course, you may use the numeric OID too
description = session.get('.1.3.6.1.2.1.1.1.0')

# Set a variable using an SNMP SET
session.set('sysLocation.0', 'The SNMP Lab')

# Perform an SNMP walk
system_items = session.walk('system')

# Each returned item can be used normally as its related type (str or int)
# but also has several extended attributes with SNMP-specific information
for item in system_items:
    print ('{oid}.{oid_index} {snmp_type} = {value}'.format(
        oid=item.oid,
        oid_index=item.oid_index,
        snmp_type=item.snmp_type,
        value=item.value
    ))

'''import netsnmp

session = netsnmp.Session(Version=2, DestHost="snmpd:1662",  Community="public")

var = netsnmp.VarList(netsnmp.Varbind('1.3.6.1.4.1.8073.1.1.4.0', 0))
#var = netsnmp.VarList('MY-MIB::myIntMibObject')
res = session.get(var)
#print("res value: ", res)
#print(res[0]*2)


#serv = "snmpd:1662"
#snmp_pass = "public"

#oid = netsnmp.VarList('IF-MIB::ifName','IF-MIB::ifDescr')
#oid = netsnmp.VarList('IF-MIB::ifHCInOctets','IF-MIB::ifDescr')
#snmp_res = netsnmp.snmpwalk(oid, Version=2, DestHost=serv, Community=snmp_pass)
for x in res:
    print("snmp_res:: ", x)

#snmpwalk -c public -v2c localhost:1662 IF-MIB::ifHCInOctets
'''