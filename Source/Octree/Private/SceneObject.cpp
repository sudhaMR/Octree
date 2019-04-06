// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneObject.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
ASceneObject::ASceneObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a static mesh component
	objectMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	//Make this component the root
	objectMeshComponent->SetupAttachment(RootComponent);

	//Create a static mesh
	objectMesh = CreateDefaultSubobject<UStaticMesh>(TEXT("StaticMesh"));
	
	//Find the static mesh in the project
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Torus.Shape_Torus'"));

	//Since unreal converts the assets into uasset files, extract the object from the uasset
	objectMesh = MeshRef.Object;

	//Set the static mesh component to be attached to the static mesh
	objectMeshComponent->SetStaticMesh(MeshRef.Object);
}

// Called when the game starts or when spawned
void ASceneObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASceneObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

