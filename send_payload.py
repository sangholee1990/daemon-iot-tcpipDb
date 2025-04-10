# cd /SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB
# python send_payload.py
# python /SYSTEMS/IOT/Roverdyn/PROJ_TCP_DB/send_payload.py

import socket

# --- 설정 ---
HOST = '49.247.41.71'  # 대상 서버 IP 주소
PORT = 9999            # 대상 서버 포트 번호
# 보낼 16진수 데이터 (공백 없이)

# 정상
hex_payload = 'FF00308207E84244574944452D30303333662D303561333737363739362D3839666634342D623762336563302D64333034303365343236323032352D30312D30312031303A30303A303041C1999A42B066664129999A4019999A6D6F76656D656E740000000000000000000000004090000042C8CCCD4039999A4290333341B2666642606666'

# 이상
# hex_payload = 'FA00308207E84244574944452D30303333662D303561333737363739362D3839666634342D623762336563302D64333034303365343236323032352D30312D30312031303A30303A303041C1999A42B066664129999A4019999A6D6F76656D656E740000000000000000000000004090000042C8CCCD4039999A4290333341B2666642606666'



# (참고) 만약 "SoF 0x00 0x00 N" 헤더가 필요하다면 아래처럼 헤더를 추가하세요.

# SoF 536F46
# 0x00 00
# 0x00 00
# N len(hex_payload)
# header_hex = '536F4600004F' # N=0x4F (길이 79) 라고 가정
# header_hex = '%s%s%s%s'.format('536F46', '00', '00',  len(hex_payload) // 2)

# sof_hex = '536F46'
# null_byte_hex = '00'
# payload_len_dec = len(hex_payload) // 2
# # payload_len_dec = 78
# length_hex = format(payload_len_dec, '02X')

# header_hex = sof_hex + null_byte_hex + null_byte_hex + length_hex
# print(f"[CHECK] len(hex_payload) : {len(hex_payload)}")
# print(f"[CHECK] payload_len_dec : {payload_len_dec}")
# print(f"[CHECK] length_hex : {length_hex}")
# print(f"[CHECK] header_hex : {header_hex}")
# print(f"[CHECK] hex_payload : {hex_payload}")

# hex_to_send = header_hex + hex_payload
hex_to_send = hex_payload
# hex_to_send = hex_payload # 이 예제에서는 헤더 없이 페이로드만 보냅니다.



try:
    # 16진수 문자열을 바이트 객체로 변환
    data_to_send = bytes.fromhex(hex_to_send)

    # 소켓 생성 및 연결
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        print(f"Connecting to {HOST}:{PORT}...")
        s.connect((HOST, PORT))
        print(f"Connected.")

        # 데이터 전송
        print(f"Sending {len(data_to_send)} bytes...")
        s.sendall(data_to_send)
        print(f"Data sent: {data_to_send.hex().upper()}")

        # (선택) 서버 응답 받기
        print("Waiting for response...")
        received_data = s.recv(1024) # 최대 1024 바이트 읽기
        if received_data:
            # print(f"Received: {received_data.hex().upper()}")
            response_text = received_data.decode('ascii')
            print(f"Response Text: {response_text}")
        else:
            print("No response or connection closed.")

except socket.error as e:
    print(f"Socket error: {e}")
except ValueError as e:
    print(f"Hex conversion error: {e}")
except Exception as e:
    print(f"An unexpected error occurred: {e}")
finally:
    print("Closing connection.")
    # 'with' 구문이 자동으로 소켓을 닫아줍니다.