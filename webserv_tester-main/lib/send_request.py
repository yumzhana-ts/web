import sys
sys.path.append('../')
import config
import socket
from http.client import HTTPResponse

def send_request(request_header):
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.settimeout(10)  # таймаут 2 секунды
    client.connect((config.SERVER_ADDR, config.SERVER_PORT))
    client.send(request_header.encode())

    # read and parse http response
    http_response = HTTPResponse(client)
    try:
        http_response.begin()
    except socket.timeout:
        print("Request timed out")
        return None
    return http_response