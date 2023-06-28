@0x86067e6a0fae5e5e;

struct PlainData{
    type @0: UInt8;
    events @1: List(PlainEvent);
}

struct PlainEvent {
    board @0 :UInt8;
    channel @1 :UInt8;
    energy @2: UInt16;
    timestamp @3: UInt64; 
}
