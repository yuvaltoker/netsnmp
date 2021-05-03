import netsnmp

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