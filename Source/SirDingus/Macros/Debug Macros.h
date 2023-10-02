#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), ImpactPoint, 25.f, 12, FColor::Red, false, 5.f);
#define DRAW_SPHERE_Persistant(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), ImpactPoint, 25.f, 12, FColor::Red, true);
#define DRAW_SPHERE_Color(Location, Colour) if (GetWorld()) DrawDebugSphere(GetWorld(), ImpactPoint, 25.f, 12, Colour, false, 5.f);

#define DEBUG_MESSAGE(Message) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, Message);