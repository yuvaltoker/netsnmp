import netsnmp

var = netsnmp.Varbind('1.3.6.1.4.1.8073.1.1.4', 0)
res = netsnmp.snmpget(var, Version=2, DestHost="snmpd:1662",  Community="public")
print(res[0])
print(res[0]*2)