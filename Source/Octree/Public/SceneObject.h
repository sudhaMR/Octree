// Fill out your copyright notice in the Description page of Project Settings.
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h "
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneObject.generated.h"

UCLASS()
class OCTREE_API ASceneObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneObject();

	UPROPERTY(EditAnywhere, Category = StaticMeshComponents)
		UStaticMesh* objectMesh;

	UPROPERTY(EditAnywhere, Category = StaticMeshComponents)
		UStaticMeshComponent* objectMeshComponent;

	FVector scenePos;

	UFUNCTION()
		void DestroyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
