#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FINNetworkComponent.h"
#include "Signals/FINSignalSender.h"
#include "FINNetworkCable.h"
#include "FINNetworkConnector.generated.h"

/**
 * This component allows the actor to get connected to a computer network.
 * It espetially allows to connect computer network cables to it.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class FICSITNETWORKS_API UFINNetworkConnector : public USceneComponent, public IFINNetworkComponent, public IFINSignalSender, public IFGSaveInterface {
	GENERATED_BODY()

protected:
	bool searchFor(TSet<const UFINNetworkConnector*>& searched, UFINNetworkConnector* obj) const;
	void removeConnector(UFINNetworkConnector* connector);

	/**
	 * The ID of this computer network component.
	 * Used to unqiuely identify it in the network.
	 * Gets automatically generated on begin play if it is nor already generated/saved.
	 */
	UPROPERTY(SaveGame)
	FGuid ID;

	/**
	 * The nick of this computer network component.
	 * Used to group components and give them an alias.
	 */
	UPROPERTY(SaveGame)
	FString Nick;

	/**
	 * Used to check if the ID is already generated.
	 */
	UPROPERTY(SaveGame)
	bool bIdCreated = false;

	/**
	 * The computer network circuit this component is connected to.
	 */
	UPROPERTY()
	UFINNetworkCircuit* Circuit = nullptr;
	
	/**
	 * The signal listeners listening to this component.
	 */
	UPROPERTY()
	TSet<FFINNetworkTrace> Listeners;

public:
	/**
	 * The maximum amount of cables you can connect to this connector.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Network|Connector")
	int MaxCables = -1;

	/**
	* The "hidden" connections to other network connectors.
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Network|Connector")
	TSet<UFINNetworkConnector*> Connections;
	
	/**
	* The connected cables to this network connector.
	*/
	UPROPERTY()
	TSet<AFINNetworkCable*> Cables;
	
	/**
	* Other components connected to this network component.
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Network|Connector")
	TSet<UObject*> Components;
	
	/**
	* The components merged into this network component.
	*/
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Network|Connector")
	TSet<UObject*> Merged;

	UFINNetworkConnector();
	~UFINNetworkConnector();

	// Begin AActor
	virtual void BeginPlay() override;
	// End AActor

	// Begin NetworkComponent
protected:
	virtual FGuid GetID_Implementation() const override;
	virtual FString GetNick_Implementation() const override;
	virtual void SetNick_Implementation(const FString& nick) override;
	virtual bool HasNick_Implementation(const FString& nick) override;
	virtual TSet<UObject*> GetMerged_Implementation() const override;
	virtual TSet<UObject*> GetConnected_Implementation() const override;
	virtual FFINNetworkTrace FindComponent_Implementation(FGuid id) const override;
	virtual UFINNetworkCircuit* GetCircuit_Implementation() const override;
	virtual void SetCircuit_Implementation(UFINNetworkCircuit* circuit) override;
	virtual void NotifyNetworkUpdate_Implementation(int type, const TSet<FFINNetworkTrace>& nodes) override;
public:
	// End NetworkComponent

	// Begin NetworkSignalSender
	virtual void AddListener_Implementation(FFINNetworkTrace listener) override;
	virtual void RemoveListener_Implementation(FFINNetworkTrace listener) override;
	virtual TSet<FFINNetworkTrace> GetListeners_Implementation() override;
	// End NetworkSignalSender

	/**
	 * adds the given connector as connection to this connector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Connector")
	void AddConnection(UFINNetworkConnector* connector);

	/**
	 * removes the given connector as connection from this connector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Connector")
	void RemoveConnection(UFINNetworkConnector* connector);

	/**
	 * adds the given network cable to this connector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Connector")
	bool AddCable(AFINNetworkCable* cable);

	/**
	 * removes the given network cable from this connector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Connector")
	void RemoveCable(AFINNetworkCable* cable);

	/**
	 * Checks if the given network cables is connected to this connector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Connector")
	bool IsConnected(UFINNetworkConnector* cable) const;

	/**
	 * Checks if the given network connector is connected to this connector.
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Connector")
	bool SearchFor(UFINNetworkConnector* conn) const;

	/**
	 * This network signals gets emit when a network change occurs.
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Signals")
	void sig_NetworkUpdate(int changeType, FString changedComponen);
};