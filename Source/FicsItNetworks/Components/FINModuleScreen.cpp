﻿#include "FINModuleScreen.h"


#include "UnrealNetwork.h"
#include "Graphics/FINGPUInterface.h"

AFINModuleScreen::AFINModuleScreen() {
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("WidgetComponent");
	WidgetComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

void AFINModuleScreen::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AFINModuleScreen, GPU);
}

void AFINModuleScreen::BeginPlay() {
	Super::BeginPlay();
	
	if (HasAuthority()) GPUPtr = GPU.Get();
	if (GPUPtr) Cast<IFINGPUInterface>(GPUPtr)->RequestNewWidget();
}

void AFINModuleScreen::EndPlay(const EEndPlayReason::Type endPlayReason) {
	Super::EndPlay(endPlayReason);
	if (endPlayReason == EEndPlayReason::Destroyed) BindGPU(FFINNetworkTrace());
}

void AFINModuleScreen::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if (HasAuthority() && (((bool)GPUPtr) != GPU.IsValid())) {
		if (!GPUPtr) GPUPtr = GPU.Get();
		OnGPUValidationChanged(GPU.IsValid(), GPUPtr);
		GPUPtr = GPU.Get();
	}
}

void AFINModuleScreen::BindGPU(const FFINNetworkTrace& gpu) {
	if (gpu.GetUnderlyingPtr().IsValid()) check(gpu->GetClass()->ImplementsInterface(UFINGPUInterface::StaticClass()))
	if (!(GPU == gpu)) {
		FFINNetworkTrace oldGPU = GPU;
		GPU = FFINNetworkTrace();
		if (oldGPU.GetUnderlyingPtr().IsValid()) Cast<IFINGPUInterface>(oldGPU.GetUnderlyingPtr().Get())->BindScreen(FFINNetworkTrace());
		GPU = gpu;
		if (gpu.GetUnderlyingPtr().IsValid()) Cast<IFINGPUInterface>(gpu.GetUnderlyingPtr().Get())->BindScreen(gpu / this);
		GPUPtr = GPU.Get();
	}
	NetMulti_OnGPUUpdate();
}

FFINNetworkTrace AFINModuleScreen::GetGPU() const {
	return GPU;
}

void AFINModuleScreen::SetWidget(TSharedPtr<SWidget> widget) {
	if (Widget != widget) Widget = widget;
	WidgetComponent->SetSlateWidget(
		Widget.IsValid() ?
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFit)
			.Content()[
				Widget.ToSharedRef()
			]
		:
			TSharedPtr<SScaleBox>(nullptr));
	OnWidgetUpdate.Broadcast();
}

TSharedPtr<SWidget> AFINModuleScreen::GetWidget() const {
	return Widget;
}

void AFINModuleScreen::RequestNewWidget() {
	if (GPUPtr) Cast<IFINGPUInterface>(GPUPtr)->RequestNewWidget();
}

void AFINModuleScreen::OnGPUValidationChanged_Implementation(bool bValid, UObject* newGPU) {
	if (bValid) {
		if (newGPU) Cast<IFINGPUInterface>(newGPU)->DropWidget();
	} else {
		Cast<IFINGPUInterface>(newGPU)->RequestNewWidget();
	}
}

void AFINModuleScreen::NetMulti_OnGPUUpdate_Implementation() {
	if (GPUPtr) {
		Cast<IFINGPUInterface>(GPUPtr)->RequestNewWidget();
	} else {
		SetWidget(nullptr);
	}
	OnGPUUpdate.Broadcast();
}
