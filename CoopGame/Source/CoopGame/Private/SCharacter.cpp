// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "GameFrameWork/PawnMovementComponent.h"
#include "Components//CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "CoopGame.h"
#include "SHealthComponent.h"
#include "SWeapon.h"
#include "SGameMode.h"
#include "Net/UnrealNetwork.h"

#include "SGameOverUserWidget.h"
#include "TextBlock.h"
#include "SPlayerState.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLSION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));	
	CameraComp->SetupAttachment(SpringArmComp);

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttackSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	if (Role == ROLE_Authority)
	{
		//Spawn a default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttackSocketName);
		}
	}
	

	
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, 
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Die
	if (Health <= 0.0f && !bDied)
	{
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		CurrentWeapon->Destroyed();
		SetLifeSpan(4.0f);
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

	/*
	if (Role == ROLE_Authority)
	{
		//���÷�
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{

			APlayerController* PC = It->Get();
			if (PC && PC->PlayerState)
			{
				if (PC->PlayerState->Score >= 20.0f)
				{
					VictorID = PC->PlayerState->PlayerId;
					GameOver(VictorID);
					PC->PlayerState->Score = 0.0f;
				}

			}
		}
	}
	*/
	
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);


	//TEST
	PlayerInputComponent->BindAction("TestCreateWidget", IE_Pressed, this, &ASCharacter::TestCreateWidget);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

//
void ASCharacter::TestCreateWidget()
{
	if (GEngine && GEngine->GameViewport)
	{
		/*
		static ConstructorHelpers::FClassFinder<UUserWidget> GameOverUserWidgetBP(TEXT("WidgetBlueprint'/Game/UI/WBP_GameOver.WBP_GameOver_C'"));
		if (GameOverUserWidgetBP.Succeeded())
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverUserWidgetBP Succeed!"));
			GameOverUserWidgetClass = GameOverUserWidgetBP.Class;
		}
		*/

		GameOverUserWidget = CreateWidget<USGameOverUserWidget>(Cast<APlayerController>(GetController()), GameOverUserWidgetClass);

		if (GameOverUserWidget != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverUserWidget AddToViewPort!"));
			//GameOverUserWidget->VictorName->SetText(FText::FromString(FString::FromInt(_VictorID)));
			GameOverUserWidget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverUserWidget is a nullptr"));
		}
	}
}

void ASCharacter::GameOver_Implementation(const int32 _VictorID)
{
	//����GameOverWidget
	if (GEngine && GEngine->GameViewport)
	{
		/*
		static ConstructorHelpers::FClassFinder<USGameOverUserWidget> GameOverUserWidgetBP(TEXT("WidgetBlueprint'/Game/UI/WBP_GameOver.WBP_GameOver_C'"));
		if (GameOverUserWidgetBP.Succeeded())
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverUserWidgetBP Succeed!"));
			GameOverUserWidgetClass = GameOverUserWidgetBP.Class;
		}
		*/
		GameOverUserWidget = CreateWidget<USGameOverUserWidget>(Cast<APlayerController>(GetController()), GameOverUserWidgetClass);

		if (GameOverUserWidget != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverUserWidget AddToViewPort!"));
			//GameOverUserWidget->VictorName->SetText(FText::FromString(FString::FromInt(_VictorID)));
			GameOverUserWidget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameOverUserWidget is a nullptr"));
		}
	}
}

bool ASCharacter::GameOver_Validate(const int32 _VictorID)
{
	return true;
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDied);
}