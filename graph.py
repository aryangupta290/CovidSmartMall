import requests
from datetime import datetime
import matplotlib.pyplot as plt
import numpy as np

uri = "http://127.0.0.1:8080/~/in-cse/in-name/total_people/node3/?rcn=4"
headers = {
    'X-M2M-ORIGIN': 'admin:admin',
    'Content-type': 'application/json'
}
response = requests.get(uri, headers=headers)
data = response.json()
x = []
y = []
for itr in data['m2m:cnt']['m2m:cin']:
    if itr['con'] == 'nan':
        continue
    # if itr['con'] == '1512':
     #   continue
    x.append(float(itr['con']))
    y.append(datetime.strptime(itr["ct"], '%Y%m%dT%H%M%S'))

for itr in x:
    print(itr)

plt.ylim((0, 100))
plt.plot(y, x)
plt.xlabel('ctime')
plt.ylabel('total_people')
plt.title('oneM2M')
plt.show()
