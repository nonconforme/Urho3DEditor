#include <Urho3D/Urho3D.h>
#include <Urho3D/Container/Str.h>

// Define Common Names to be used throughout the application
// This helps to consolidate "named" objects such as Nodes
#define NAME(NameID, Name) static const Urho3D::String NameID(#Name);

// Connection identity attributes
NAME(CON_USERNAME, Username)

// Named Event field modifiers
NAME(EF_DISCONNECT_REASON, DisconnectReason)

//Reserved Rooms
NAME(R_LOBBY, Lobby) // Represents the main lobby
NAME(R_TEST, Test) // Special room only allowed to create/join by administrators

// Named Nodes
NAME(N_PLAYERS, Players)
NAME(N_PLAYFIELD, PlayField)

// Named attributes
NAME(ATTR_NUM_PLAYERS, NumPlayers)
NAME(ATTR_ROOM_ID, RoomID)
NAME(ATTR_ROOM_NAME, RoomName)