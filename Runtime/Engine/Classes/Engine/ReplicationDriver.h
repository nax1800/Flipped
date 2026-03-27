#pragma once

class ReplicationDriver
{
public:
	static int32 ServerReplicateActors(UReplicationDriver* Context, float DeltaSeconds);
};