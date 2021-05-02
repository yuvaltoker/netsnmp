import netsnmp

var = netsnmp.VarList('MY-MIB::myIntMibObject', 'MY-MIB::myStringMibObject')
res = netsnmp.snmpwalk(var, Version=2, DestHost="snmpd:1662",  Community="public")
#res = netsnmp.snmpset(var, Version=2, DestHost="snmpd:1662",  Community="public")
#print(res[0])
#print(res[0]*2)


#serv = "snmpd:1662"
#snmp_pass = "public"

#oid = netsnmp.VarList('IF-MIB::ifName','IF-MIB::ifDescr')
#oid = netsnmp.VarList('IF-MIB::ifHCInOctets','IF-MIB::ifDescr')
#snmp_res = netsnmp.snmpwalk(oid, Version=2, DestHost=serv, Community=snmp_pass)
for x in res:
    print("snmp_res:: ", x)

#snmpwalk -c public -v2c localhost:1662 IF-MIB::ifHCInOctets