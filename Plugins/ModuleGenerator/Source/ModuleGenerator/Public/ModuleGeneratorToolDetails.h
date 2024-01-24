// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "ModuleEditorTool.h"
#include "DetailWidgetRow.h"

/**
 * 自定义UModuleEditorTool的显示样式
 */
class MODULEGENERATOR_API FModuleGeneratorToolDetails : public IDetailCustomization
{
public:
	/**
	 * 自定义Slate样式
	 * @param DetailBuilder DetailBuilder实例
	 */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	
	/**
	 * 创建FModuleGeneratorToolDetails实例对象
	 * @return FModuleGeneratorToolDetails实例对象
	 */
	static TSharedRef<IDetailCustomization> MakeInstance();
	
	typedef TSharedPtr<FString> FComboItemType;
	
	/**
	 * UModuleEditorTool实例对象指针
	 */
	TWeakObjectPtr<UModuleEditorTool> Tool;
	
	/**
	 * 当前选择的插件
	 */
	FComboItemType CurrentSelectedPlugin;
	
	/**
	 * 所有可以选择的插件
	 */
	TArray<FComboItemType> Options;
	
	/**
	 * 获取当前选择的插件
	 * @return 插件名字
	 */
	FText GetCurrentItemLabel() const;
	
	/**
	 * 当前选择的插件发生变化
	 * @param NewValue 当前选择的插件
	 */
	void OnSelectionChanged(FComboItemType NewValue, ESelectInfo::Type);
	
	/**
	 * 生成SWidget对象
	 * @param InOption 当前选择的插件
	 * @return SWidget对象
	 */
	TSharedRef<SWidget> MakeWidgetForOption(FComboItemType InOption);
	
	/**
	 * 模块类型发生变化
	 */
	void OnTemplateChange() const;
	
	/**
	 * 查找所有的插件
	 */
	void FillComboBoxWithPluginNames();
	
	/**
	 * 是否显示插件选择项
	 * @return 可见性
	 */
	EVisibility GetBasicParentClassVisibility() const;
	
	/**
	 * Generate New Module的点击事件，生成模块的真正逻辑实现
	 * @return FReply
	 */
	FReply GenerateNewModuleCall() const;
	
	/**
	 * 是否显示错误信息
	 * @return 可见性
	 */
	EVisibility GetNameErrorLabelVisibility() const;
	
	/**
	 * 获取错误信息的内容
	 * @return 错误信息
	 */
	FText GetNameErrorLabelText() const;
	
	/**
	 * 根据输入的模块名字，判断是否要显示错误信息
	 */
	void OnChangeNewModuleName();
	
	/**
	 * 缓存上次的错误信息
	 */
	FString LastErrorText;
	
	/**
	 * 是否有错误
	 */
	bool WasError = false;
};