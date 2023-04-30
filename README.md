# R2_BLE_remote
https://sdk.sphero.com/docs/api_spec/general_api#packet-encoding


# Protocol Definition
All Sphero API packets share a single structure defined below. Packets start/end with the special SOP/EOP control bytes. These control byte values are not allowed unencoded anywhere else in the packet (see: Packet Encoding). Packets are classified as either commands or responses, distinguished by a bit flag. Commands may be sent at any time by any node. Commands may optionally request a response from the receiver using a bit flag.
Packets can be directed to particular nodes using the “target ID” (TID) and “source ID” (SID). The TID and SID are each composed of two parts, a “port ID” and a “node ID” (See: Sending Information via Channels).
All packets have a two-byte ID code, separated into the “device ID” (DID) and “command ID” (CID). These bytes specify the command to execute. When sending a command, the sender uses the “sequence number” (SEQ) as a handle to link a particular command packet to its response. Responses echo the DID, CID, and SEQ to help the sender fully identify the corresponding command packet.
Responses contain a special “error code” field in the header that expresses the result of the corresponding command (see: Error Codes).
Both commands and responses may contain a data payload. In principle, this payload may be any size, though in practice the payload is limited by the receiver’s buffers. For forwarded commands, the data size may also be limited by intermediate nodes’ buffer sizes, depending on the implementation.
Finally, all packets contain a single byte checksum, which is computed over all other bytes in the packet (excluding SOP and EOP) before payload encoding.

# Packet Structure
The Sphero API packet structure is shown below. All items are a single byte, except DATA, which is zero or more bytes.

| Abreviation | Name | Description |
| ----- | ----------- | --------------- |
| SOP:	| Start of Packet	| Control byte identifying the start of the packet |
| FLAGS: | Packet Flags	| Bit-flags that modify the behavior of the packet |
| TID:	| Target ID	| Address of the target, expressed as a port ID (upper nibble) and a node ID (lower nibble). (Optional) |
| SID:	| Source ID	| Address of the source, expressed as a port ID (upper nibble) and a node ID (lower nibble). (Optional) |
| DID:	| Device ID	| The command group ("virtual device") of the command being sent |
| CID:	| Command ID |	The command to execute |
| SEQ:	| Sequence Number |	The token used to link commands with responses |
| ERR:	| Error Code |	Command error code of the response packet (optional) |
| DATA: | Message Data |	Zero or more bytes of message data |
| CHK:	| Checksum	| The sum of all bytes (excluding SOP & EOP) mod 256, bit-inverted |
| EOP:	| End of Packet | 	Control byte identifying the end of the packet |

# Packet Encoding
To avoid misinterpretations of the packet structure, the SOP and EOP bytes are never allowed in the rest of the packet. If these byte values are necessary in the payload, they are encoded before transmission and decoded by the receiving parser. The encoding method is an extension of SLIP encoding, using a two-byte escape sequence to replace special characters. This method necessitates a third special character, “escape” (ESC). The three special characters are encoded by prepending the ESC character and changing the original values to corresponding “escaped” values. These six values are shown in the table below.

| Abbreviation | Description | Value |
| ------------ | ----------------- | ----- |
| ESC          | Escape            |  0xAB |		
| SOP	       | Start of Packet   |  0x8D |
| EOP	       | End of Packet     |  0xD8 |
| ESC_ESC	   | Escaped "Escape"  | Ox23 |
| ESC_SOP	   | Escaped "Start of Packet" | 	0x05 |
| ESC_EOP	   | Escaped "End of Packet"  | 	0x50| 

When the ESC, SOP, or EOP bytes are needed in the payload, they are encoded into (or decoded from) two-byte escape sequences as follows:

| Encoding | Decoding |
| ------------------ | ------------------ | 	
| ESC → ESC, ESC_ESC | ESC, ESC_ESC → ESC |
| SOP → ESC, ESC_SOP | ESC, ESC_SOP → SOP |
| EOP → ESC, ESC_EOP | ESC, ESC_EOP → EOP |

The values for ESC, SOP, and EOP were specifically selected to minimize the cost of encoding. Additionally, these values were selected to provide a simple relationship between control values and their corresponding escaped value (unencoded value = escaped value | 0x88).

# Flags
API flags are used to modify the behavior of the API system. The available API flags are listed below. Bits are listed 0 to 7, where 0 is the least significant bit.

| Bit |Flag | 	Bit = 1	| Bit = 0 |
| - | --------- | --------- | ------ | 
| 0	| Packet is a Response| 	Packet is a response. This implies that the packet has the error code byte in the header.	|Packet is a command.|
| 1	| Request Response|	Request response to a command (only valid if the packet is a command).	| Do not request any response.|
| 2	| Request Only Error Response	|Request response only if command results in an error (only valid if packet is a command and "Request Response" flag is set).|	Do not request only error responses.|
| 3	| Packet is Activity	|This packet counts as activity. Reset receiver's inactivity timeout.	| Do not reset receiver's inactivity timeout. |
| 4	| Packet has Target ID	| Packet has Target ID byte in header.| 	Packet has no specified target.| 
| 5	| Packet has Source ID	| Packet has Source ID byte in header.	| Packet has no specified source.| 
| 6	| Currently Unused	| n/a	| n/a| 
| 7	| Extended Flags |	The next header byte is extended flags.	| This is the last flags byte.| 

# Error Codes
| Flag | Value | Description |
| ------------------ | -------| -------------------- | 	
| Success |	0x00 |	Command executed successfully
| Bad Device ID  |	0x01 |	Device ID is invalid (or is invisible with current permissions)
| Bad Command ID |	0x02 |	Command ID is invalid (or is invisible with current permissions)
| Not Yet Implemented |	0x03 |	Command is not yet implemented or has a null handler
| Command is Restricted | 0x04 | Command cannot be executed in the current state or mode
| Bad Data Length |	0x05 |	Payload data length is invalid
| Command Failed |	0x06 |	Command failed to execute for a command-specific reason
| Bad Parameter Value |	0x07 |	At least one data parameter is invalid
| Busy | 0x08 |	The operation is already in progress or the module is busy
| Bad Target ID	 |	0x09 |	 Target does not exist
| Target Unavailable |	0x0A | Target exists but is unavailable (e.g., it is asleep or disconnected)
| Currently Unused | 0x0B – 0xFF |	Currently unused

# Payload Data
Supported Types
| Type | Size(s)[bits] | Encoding |
| ----- | -------| ------| 
| Unsigned Integer |	8, 16, 32	||
| Signed Integer |	8, 16, 32	| two's compliment|
| Floating Point |	32	| IEEE 754|
| Bool |	8	| zero = false, non-zero = true|
|Byte Array |	8 * n	||
|String	| 8 * n	| generally null-terminated|

# Endianness
In the Sphero API, all multi-byte words are big-endian. Byte arrays and strings are sent in index-order (lowest indexed item first).

Standard (Preferred) Units
We aspire towards using the standard units below to make it easier on both you and our own team to be able to develop with confidence and consistency across our commands. We'll do our best not to let you down on this front and to adhere to these units for all of the API commands we all have the pleasure of utilizing.

- Distance: Meters
- Time: Seconds
- Speed: Meters / Second
- Angle: Degrees
- Velocity: Degrees / Second
- Acceleration: G
- Temperature: C
- RSSI: dB
- Quaternion: normalized
- Audio Volume: (TBD)

# Packet Addressing and Routing
This API utilizes in-protocol packet routing, allowing peers to directly target each other’s API nodes. In addition to more clearly specifying communication paths, this feature lets processors host identical commands (e.g., “Get Main App Version” or “Echo”).

# The Key Elements
The Sphero API is built on a system of nodes, which send and receive information. Each node is contained in a system and a system can contain more than one node. As an example, the RVR+/RVR is a system that contains two nodes, (1) the Bluetooth SOC, which also handles the power system, indications, the color sensor, and the ambient light sensor and (2) a general-purpose microcontroller which handles the control system, IR communication, USB, and the IMU.