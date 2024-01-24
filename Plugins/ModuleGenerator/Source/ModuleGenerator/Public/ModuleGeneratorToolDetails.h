// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "ModuleEditorTool.h"
#include "DetailWidgetRow.h"

/**
 * �Զ���UModuleEditorTool����ʾ��ʽ
 */
class MODULEGENERATOR_API FModuleGeneratorToolDetails : public IDetailCustomization
{
public:
	/**
	 * �Զ���Slate��ʽ
	 * @param DetailBuilder DetailBuilderʵ��
	 */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	
	/**
	 * ����FModuleGeneratorToolDetailsʵ������
	 * @return FModuleGeneratorToolDetailsʵ������
	 */
	static TSharedRef<IDetailCustomization> MakeInstance();
	
	typedef TSharedPtr<FString> FComboItemType;
	
	/**
	 * UModuleEditorToolʵ������ָ��
	 */
	TWeakObjectPtr<UModuleEditorTool> Tool;
	
	/**
	 * ��ǰѡ��Ĳ��
	 */
	FComboItemType CurrentSelectedPlugin;
	
	/**
	 * ���п���ѡ��Ĳ��
	 */
	TArray<FComboItemType> Options;
	
	/**
	 * ��ȡ��ǰѡ��Ĳ��
	 * @return �������
	 */
	FText GetCurrentItemLabel() const;
	
	/**
	 * ��ǰѡ��Ĳ�������仯
	 * @param NewValue ��ǰѡ��Ĳ��
	 */
	void OnSelectionChanged(FComboItemType NewValue, ESelectInfo::Type);
	
	/**
	 * ����SWidget����
	 * @param InOption ��ǰѡ��Ĳ��
	 * @return SWidget����
	 */
	TSharedRef<SWidget> MakeWidgetForOption(FComboItemType InOption);
	
	/**
	 * ģ�����ͷ����仯
	 */
	void OnTemplateChange() const;
	
	/**
	 * �������еĲ��
	 */
	void FillComboBoxWithPluginNames();
	
	/**
	 * �Ƿ���ʾ���ѡ����
	 * @return �ɼ���
	 */
	EVisibility GetBasicParentClassVisibility() const;
	
	/**
	 * Generate New Module�ĵ���¼�������ģ��������߼�ʵ��
	 * @return FReply
	 */
	FReply GenerateNewModuleCall() const;
	
	/**
	 * �Ƿ���ʾ������Ϣ
	 * @return �ɼ���
	 */
	EVisibility GetNameErrorLabelVisibility() const;
	
	/**
	 * ��ȡ������Ϣ������
	 * @return ������Ϣ
	 */
	FText GetNameErrorLabelText() const;
	
	/**
	 * ���������ģ�����֣��ж��Ƿ�Ҫ��ʾ������Ϣ
	 */
	void OnChangeNewModuleName();
	
	/**
	 * �����ϴεĴ�����Ϣ
	 */
	FString LastErrorText;
	
	/**
	 * �Ƿ��д���
	 */
	bool WasError = false;
};