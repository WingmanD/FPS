#include "MainMenuWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (_joinButton != nullptr)
	{
		_joinButton->OnClicked.AddDynamic(this, &UMainMenuWidget::Join);
	}

	if (_quitButton != nullptr)
	{
		_quitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::QuitGame);
	}
}

void UMainMenuWidget::Join()
{
	if (_ipAddressTextBox == nullptr)
	{
		return;
	}

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController == nullptr)
	{
		return;
	}

	playerController->ClientTravel(_ipAddressTextBox->GetText().ToString(), TRAVEL_Absolute);
}

void UMainMenuWidget::QuitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}
