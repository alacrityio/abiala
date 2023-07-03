## abiala

Binary <> JSON conversion using ABIs. Compatible with languages which can interface to C; see [src/abiala.h](src/abiala.h).

Alpha release. Feedback requested.

## Packing transactions

1. Create a context: `abiala_create`
1. Use `abiala_set_abi` to load [alajs/src/transaction.abi](https://github.com/ALADINIO/alajs/blob/master/src/transaction.abi) into contract 0.
1. Use `abiala_set_abi` to load the contract's ABI.
1. Use `abiala_json_to_bin` and `abiala_get_bin_hex` to convert action data to hex. Use `abiala_get_type_for_action` to get the action's type.
1. Use `abiala_json_to_bin` and `abiala_get_bin_hex` to convert transaction to hex. Use `contract = 0` and `type = abiala_string_to_name(context, "transaction")`.
1. Destroy the context: `abiala_destroy`

## Usage note

abiala expects object attributes to be in order. It will complain about missing attributes if they are out of order.

## Example data

Example action data for `abiala_json_to_bin`:

```
{
    "from": "useraaaaaaaa",
    "to": "useraaaaaaab",
    "quantity": "0.0001 SYS",
    "memo": ""
}
```

Example transaction data for `abiala_json_to_bin`:

```
{
    "expiration": "2018-06-27T20:33:54.000",
    "ref_block_num": 45323,
    "ref_block_prefix": 2628749070,
    "max_net_usage_words": 0,
    "max_cpu_usage_ms": 0,
    "delay_sec": 0,
    "context_free_actions": [],
    "actions": [{
        "account": "alaio.token",
        "name": "transfer",
        "authorization":[{
            "actor":"useraaaaaaaa",
            "permission":"active"
        }],
        "data":"608C31C6187315D6708C31C6187315D60100000000000000045359530000000000"
    }],
    "transaction_extensions":[]
}
```

## Ubuntu 16.04 with gcc 8.1.0

- Install these. You may have to build them yourself from source or find a PPA. Make them the default.
  - gcc 8.1.0
  - cmake 3.11.3
- `sudo apt install libboost-dev libboost-date-time-dev`
- remove this from CMakeLists.txt (2 places): `-fsanitize=address,undefined`

```
mkdir build
cd build
cmake ..
make
./test
```

## License

[MIT](./LICENSE)
