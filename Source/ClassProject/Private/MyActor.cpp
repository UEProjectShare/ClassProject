// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	A = 200;
	B = 1.0f;

}

void AMyActor::MyFunc()
{
	UE_LOG(LogTemp, Warning, TEXT("Hello World"));
}

FString AMyActor::BlueprintNativeEventFunction_Implementation(AActor* InActor)
{
	return InActor->GetName();
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	Test();
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

