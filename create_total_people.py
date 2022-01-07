from onem2m import *
uri_cse = "http://127.0.0.1:8080/~/in-cse/in-name"

ae = "total_people"
cnt = "node3"

uri_ae = uri_cse + "/" + ae
uri_cnt = uri_ae + "/" + cnt
create_ae(uri_cse, ae)
create_cnt(uri_ae, cnt)
create_data_cin(uri_cnt, "1512")
# delete_ae(uri_ae)
