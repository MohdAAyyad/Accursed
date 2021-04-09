// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory.h>

/**
 * 
 */
class TOPDOWNHS_API Intermediate
{
public:
	Intermediate();
	~Intermediate();

protected:

	static TUniquePtr<Intermediate, TDefaultDelete<Intermediate>> instance;

public:
	static Intermediate* GetInstance();

	int currentWaveID;
};
