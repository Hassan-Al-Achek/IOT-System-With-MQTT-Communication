import binascii
import struct
import time
from digi import ble
from machine import Pin


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


# selectOptionButton supposed to be the user button
# dio4
# selectOptionButton = Pin(Pin.board.D0, Pin.IN)

# switchMenuOptionButton supposed to be Commissioning button
# Clicking switchMenuOptionButton
# switchMenuOptionButton = Pin("8", Pin.IN)

selectOptionButton = False
switchMenuOptionButton = True

# Change these two variables to your device's address and address type.
# This sample expects a Thunderboard React to be used
# The address and address type can be discovered using ble.gap_scan().
ESP32_ADDRESS = "24:62:AB:D2:82:86"
address_type = ble.ADDR_TYPE_PUBLIC

# Put address into bytes object (without colons)
address = binascii.unhexlify(ESP32_ADDRESS.replace(':', ''))


service_uuid = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
characteristic_uuid = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
characteristic_uuid_1 = "f0964972-6d7f-11ec-90d6-0242ac120003"


ble.active(True)
print("Attempting connection to: {}".format(ESP32_ADDRESS))

with ble.gap_connect(address_type, address) as conn:
    print("connected")

    service_characteristic = get_characteristics_from_uuids(
        conn, service_uuid, characteristic_uuid)[0]

    service_characteristic_1 = get_characteristics_from_uuids(
        conn, service_uuid, characteristic_uuid_1)[0]

    while True:
        # if switchMenuOptionButton.value():
        if switchMenuOptionButton:
            properties = service_characteristic[2]
            if properties & ble.PROP_WRITE:
                menu = conn.gattc_read_characteristic(service_characteristic)
                print(menu.decode('utf-8'))
                conn.gattc_write_characteristic(service_characteristic, b'HIGH')

        # if selectOptionButton.value():
        if selectOptionButton:
            properties = service_characteristic_1[2]
            if properties & ble.PROP_WRITE:
                resultOption = conn.gattc_read_characteristic(service_characteristic_1)
                print(resultOption.decode('utf-8'))
                conn.gattc_write_characteristic(service_characteristic_1, b'HIGH')

        time.sleep(2)
