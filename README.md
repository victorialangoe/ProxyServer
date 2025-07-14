# ProxyServer

A TCP proxy server implementation for IN2140 (Network Programming) at the University of Oslo. This system demonstrates network programming concepts including client-server architecture, socket programming, and protocol conversion.

## Overview

The ProxyServer system consists of four main components:

1. **proxy** - The main proxy server that handles client connections and message forwarding
2. **xmlSender** - Client that sends XML-formatted records to the proxy
3. **binSender** - Client that sends binary-formatted records to the proxy
4. **anyReceiver** - Client that receives records in either XML or binary format

## Architecture

The proxy server acts as an intermediary that:
- Accepts connections from multiple senders and receivers
- Converts between XML and binary message formats
- Routes messages from senders to appropriate receivers based on destination
- Maintains a linked list of connected clients

### Message Flow
1. Senders (xmlSender/binSender) connect to proxy and send records
2. Proxy determines the format and routing requirements
3. Proxy converts messages to the appropriate format if needed
4. Proxy forwards messages to the correct receiver(s)
5. Receivers (anyReceiver) receive and process the messages

## Building the Project

Use the provided Makefile to build all components:

```bash
make
```

This will create four executables:
- `proxy`
- `xmlSender`
- `binSender`
- `anyReceiver`

To clean build artifacts:
```bash
make clean
```

## Usage

### 1. Start the Proxy Server

```bash
./proxy <port>
```

**Parameters:**
- `<port>` - Port number where the proxy will listen for connections

**Example:**
```bash
./proxy 8080
```

### 2. Start XML Sender

```bash
./xmlSender <id> <proxyip> <proxyport>
```

**Parameters:**
- `<id>` - Single capital letter (A, B, C, ...) identifying this sender
- `<proxyip>` - IP address of the proxy server (e.g., 127.0.0.1)
- `<proxyport>` - Port number of the proxy server

**Example:**
```bash
./xmlSender A 127.0.0.1 8080
```

### 3. Start Binary Sender

```bash
./binSender <id> <proxyip> <proxyport>
```

**Parameters:**
- `<id>` - Single capital letter (A, B, C, ...) identifying this sender
- `<proxyip>` - IP address of the proxy server
- `<proxyport>` - Port number of the proxy server

**Example:**
```bash
./binSender B 127.0.0.1 8080
```

### 4. Start Any Receiver

```bash
./anyReceiver <id> <type> <proxyip> <proxyport> <waittime>
```

**Parameters:**
- `<id>` - Single capital letter (A, B, C, ...) identifying this receiver
- `<type>` - Format preference: 'X' for XML, 'B' for binary
- `<proxyip>` - IP address of the proxy server
- `<proxyport>` - Port number of the proxy server
- `<waittime>` - Timeout in seconds to wait for new data

**Example:**
```bash
./anyReceiver X X 127.0.0.1 8080 30
```

## Data Formats

### XML Format
Records are sent as XML documents with the following structure:
```xml
<record>
  <source="A" />
  <dest="X" />
  <username="example" />
  <id="1234" />
  <group="5678" />
  <semester="9" />
</record>
```

### Binary Format
Records are sent in a compact binary format with the same information.

## Testing

The project includes several test cases in directories named `test-*`. Each test directory contains:
- Input files (A.xml, B.xml, A.bin, B.bin)
- Expected output files (X.xml-expected, Y.bin-expected, etc.)
- RunTest.sh script to execute the test

To run a specific test:
```bash
cd test-1-xml-to-xml
./RunTest.sh
```

## Example Session

1. Start the proxy server:
   ```bash
   ./proxy 8080
   ```

2. In another terminal, start a receiver:
   ```bash
   ./anyReceiver X X 127.0.0.1 8080 60
   ```

3. In a third terminal, start a sender:
   ```bash
   ./xmlSender A 127.0.0.1 8080
   ```

The sender will read from file `A.xml` and send records to the proxy, which will forward them to the receiver identified as 'X'.

## Implementation Details

- **Connection Management**: Uses TCP sockets with select() for multiplexing
- **Protocol Detection**: Clients send format type and ID in the first two bytes
- **Message Routing**: Based on destination field in records
- **Format Conversion**: Automatic conversion between XML and binary formats
- **Resource Management**: Proper cleanup of client connections and memory

## File Structure

- `proxy.c/h` - Main proxy server implementation
- `connection.c/h` - TCP connection utilities
- `record.c/h` - Record data structure and operations
- `recordToFormat.c/h` - Convert records to XML/binary format
- `recordFromFormat.c/h` - Parse records from XML/binary format
- `linkedlist.c/h` - Client list management
- `xmlfile.c/h` - XML file operations
- `binfile.c/h` - Binary file operations
- `*Sender.c` - Client sender implementations
- `anyReceiver.c` - Client receiver implementation
