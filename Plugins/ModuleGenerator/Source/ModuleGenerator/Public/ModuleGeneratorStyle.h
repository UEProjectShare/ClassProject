// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * ��ʽ
 */
class FModuleGeneratorStyle
{
public:

	/**
	 * ��ʼ��
	 */
	static void Initialize();

	/**
	 * ж��
	 */
	static void Shutdown();
	
	/**
	 * ���¼�������
	 */
	static void ReloadTextures();
	
	/**
	 * ��ȡSlate style
	 * @return Slate style
	 */
	static const ISlateStyle& Get();

	/**
	 * ��ȡSlate style����
	 * @return Slate style����
	 */
	static FName GetStyleSetName();

private:

	/**
	 * ����SlateStyleSet
	 * @return SlateStyleSet
	 */
	static TSharedRef<class FSlateStyleSet> Create();

	/**
	 * SlateStyleSetʵ��
	 */
	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};