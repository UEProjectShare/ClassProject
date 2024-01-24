// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * 样式
 */
class FModuleGeneratorStyle
{
public:

	/**
	 * 初始化
	 */
	static void Initialize();

	/**
	 * 卸载
	 */
	static void Shutdown();
	
	/**
	 * 重新加载纹理
	 */
	static void ReloadTextures();
	
	/**
	 * 获取Slate style
	 * @return Slate style
	 */
	static const ISlateStyle& Get();

	/**
	 * 获取Slate style名字
	 * @return Slate style名字
	 */
	static FName GetStyleSetName();

private:

	/**
	 * 创建SlateStyleSet
	 * @return SlateStyleSet
	 */
	static TSharedRef<class FSlateStyleSet> Create();

	/**
	 * SlateStyleSet实例
	 */
	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};