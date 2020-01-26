# -*- coding: utf-8 -*-
"""
Created on Sat Aug 17 14:46:09 2019

@author: Sikker Rosendal
This code just runs on a computer or similar
It retrieves BTC price data from CoinBase so the ESP8266 doesn't need to
ESP8266 could do it, but this way ESP8266 only needs to manage connections to Thingspeak servers
"""

import httplib2
import urllib
import time
import cbpro

public_client = cbpro.PublicClient()
key = 'PLACE YOUR THINGSPEAK WRITE API KEY FOR BTC CHANNEL HERE'  # Thingspeak API key
delay = 20
 
def updatebtc():
    try:
        btc = public_client.get_product_ticker(product_id='BTC-USD')['price']
        params = urllib.parse.urlencode({'field1': btc, 'key':key }) 
        headers = {"Content-type": "application/x-www-form-urlencoded","Accept": "text/plain"}
        conn = httplib2.HTTPConnectionWithTimeout("api.thingspeak.com:80")
        
        conn.request("POST", "/update", params, headers)
        response = conn.getresponse()
        print(response.status, response.reason)
        #data = response.read()
        #print(data)
        conn.close()
    except:
        print("connection failed")

if __name__ == "__main__":
	while True:
		updatebtc()
		time.sleep(delay)