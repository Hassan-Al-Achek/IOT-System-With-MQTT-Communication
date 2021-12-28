import binascii
import struct
import time
from digi import ble


def get_characteristics_from_uuids(connection, service_uuid, characteristic_uuid):
    services = list(connection.gattc_services(service_uuid))
    if len(services):
        # Assume that there is only one service per UUID, take the first one
        my_service = services[0]
        characteristics = list(connection.gattc_characteristics(
            my_service, characteristic_uuid))
        return characteristics
    # Couldn't find specified characteristic, return an empy list
    return []


# Change these two variables to your device's address and address type.
# This sample expects a Thunderboard React to be used
# The address and address type can be discovered using ble.gap_scan().
THUNDERBOARD_ADDRESS = "24:62:AB:D2:82:86"
address_type = ble.ADDR_TYPE_PUBLIC

# Put address into bytes object (without colons)
address = binascii.unhexlify(THUNDERBOARD_ADDRESS.replace(':', ''))

# The service and characteristic UUIDs
io_service_uuid = 0x1815
io_characteristic_uuid = 0x2A56


service_uuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
characteristic_uuid = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

ble.active(True)
print("Attempting connection to: {}".format(THUNDERBOARD_ADDRESS))

with ble.gap_connect(address_type, address) as conn:
    print("connected")

    hello_characteristic = get_characteristics_from_uuids(
        conn, service_uuid, characteristic_uuid)[0]

    # while True:
    properties = hello_characteristic[2]
    print(properties)
    print(ble.PROP_WRITE)
    if properties & ble.PROP_WRITE:
        helloMessage = conn.gattc_read_characteristic(hello_characteristic)
        print(helloMessage)

    conn.gattc_write_characteristic(characteristic_uuid, b'Hassan')
    time.sleep(1)