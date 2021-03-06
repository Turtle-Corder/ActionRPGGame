#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAEffects.generated.h"
/*
	Random idea, so I wont forget it.

	Add scoped attributes, (as template).
	Scoped attribute, will contain names of all attributes, which are going to be needed
	in final calculation of attribute modification. It does not contain information on how
	these attribute are going to be used, only that these attributes are needed.

	Since, there is myriad of possible attributes combinations, we need some way to template,
	which attributes are needed for which modification, so designer can simply select,
	template from list.
	It can be class (UObject), or something else. 
	
	*/
/*
*/
DECLARE_MULTICAST_DELEGATE_OneParam(FGASimpleEffectDelegate, const FGAEffectHandle&);
USTRUCT(BlueprintType)
struct FGAEffectDuration
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	-1 infinite duration, 0-no duration >0 - set duration
	*/
	UPROPERTY(EditAnywhere, Category = "Duration")
		float Duration;
	/*
	<=0 - no period >0 - set period
	*/
	UPROPERTY(EditAnywhere, Category = "Duration")
		float Period;
};
/*
Encapsulates effect name into struct.
We will use to uniqely identify effects.

Idea is simple:
1. Generic effects, will have generic names (like bleed, burnining etc).
2. Specific effects, will derive name from ability/weapon/whatever they are applied from.
*/
USTRUCT(BlueprintType)
struct FGAEffectName
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	If true, use custom name.
	*/
	UPROPERTY(EditAnywhere)
		bool CustomName;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "CustomName"), Category = "Base")
		FName EffectName;

	FGAEffectName()
		: EffectName(NAME_None)
	{}
	FGAEffectName(const FName& EffectNameIn)
		: EffectName(EffectNameIn)
	{}

	const bool operator==(const FGAEffectName& OtherIn) const
	{
		return OtherIn.EffectName == EffectName;
	}

	friend uint32 GetTypeHash(const FGAEffectName& EffectNameIn)
	{
		return ::GetTypeHash(EffectNameIn.EffectName);
	}
};

struct GAMEATTRIBUTES_API FGAModifierStack
{
	float Additive;
	float Subtractive;
	float Multiply;
	float Divide;

	void Add(const FGAModifierStack& StackIn)
	{
		Additive += StackIn.Additive;
		Subtractive += StackIn.Subtractive;
		Multiply += StackIn.Multiply;
		Divide += StackIn.Divide;
	}

	FGAModifierStack()
		: Additive(0),
		Subtractive(0),
		Multiply(0),
		Divide(0)
	{};
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectModifierSpec
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Type of calculation we want to perform for this Magnitude.
	*/
	UPROPERTY(EditAnywhere)
		EGAMagnitudeCalculation CalculationType;

	/*
		Attribute on effect which will be modified.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	/*
		How Attribute Will be modified
	*/
	UPROPERTY(EditAnywhere)
		EGAAttributeMod Mod;
	/*
		Outgoing - are usually applied from Instigator, when modified effect originated from.
		Incoming - are usually applied from Target

		If outgoing mod is subtraction or PercentageReduce, it will decrease amount
		of attribute change done by Instigator (ie. damage done, life steal, etc).

		If incoming mod is additive or PercentageIncrease, it will increase amount of
		attribute change done to Target (ie. higher damage taken).

		Side note. Should probabaly change names to Outgoing/Incoming, instead of Offensive/Defensive.
	*/
	UPROPERTY(EditAnywhere)
		EGAModifierType ModifierType;

	UPROPERTY(EditAnywhere)
		FGADirectModifier DirectModifier;
	/*
		Simple calculation based on attribute:
		(Coefficient * (PreMultiply + AttributeValue) + PostMultiply) * PostCoefficient

		There is no any magic manipulation, it straight off pull attribute from selected source,
		and make this operation on it.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttributeBasedModifier AttributeBased;
	/*
		Get value from selected CurveTable, based on selected attribute value.
	*/
	UPROPERTY(EditAnywhere)
		FGACurveBasedModifier CurveBased;
	/*
		Provide custom calculation class.
	*/
	UPROPERTY(EditAnywhere)
		FGACustomCalculationModifier Custom;
};

USTRUCT(BlueprintType)
struct FGAEffectPolicy
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		EGAEffectType Type;
	UPROPERTY(EditAnywhere)
		EGAEffectStacking Stacking;
	UPROPERTY(EditAnywhere)
		EGAEffectAggregation Aggregation;
};

USTRUCT(BlueprintType)
struct FGAAttributeEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Modifier for attributes, when effect is initially applied.

		Instant application effects, do use only this property.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		TArray<FGAAttributeModifier> InitialAttributes;
	/*
		Modifier which is applied for effect duration.
		It's removed when effect is removed/expire.

		Duration effects should only be used to modify Complex Attributes!
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		TArray<FGAAttributeModifier> DurationAttributes;
	/*
		Modifier applied when effect Ticks (on period interval).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		TArray<FGAAttributeModifier> PeriodAttributes;
	/*
		Modifier applied when effect is removed from target.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		TArray<FGAAttributeModifier> RemovedAttributes;
	/*
		Modifier applied when Effect naturally expire.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		TArray<FGAAttributeModifier> ExpiredAttributes;
};

USTRUCT(BlueprintType)
struct FGAConditonalEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Target, must these tags present, to apply these effects.
	*/
	UPROPERTY(EditAnywhere, Category = "Condition")
		FGameplayTagContainer RequiredTags;
	/*
		Instanced is hack, to allow editing effect classes directly inside editor.
		Please note that when you do it you will change all blueprints of this class.
		If you want to have specific blueprint you will need to create it!.


	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Effects")
		TArray<TSubclassOf<class UGAEffectSpecification>> Effects;
};
/*
	Base class for all active effects.
	I'm not sure if I want it
*/
USTRUCT()
struct FGAActiveEffectBase
{
	GENERATED_USTRUCT_BODY()
};

/*
	Modifiers, are special types of effects. You can think of them as buff/debuff.
	Modifier never change or modify character attributes directly (as a attributes defined in
	UGAAttributesBase.
	Instead modifiers create "virtual" attributes based on GameplayTags.

	When effect is applied to actor, tags of applied effect are check, for modifiers,
	and all bonuses for qualifable modifers (those which match tags), are sumed up, but are not
	automatically applied.
	What you do with these bonuses is up to you, by overriding UGACalculation class.
	Here you can define how bonuses from attributes and modifiers  will stack togather, to modify effect.
*/
USTRUCT()
struct FGAActiveModifier
{
	GENERATED_USTRUCT_BODY()
};

/*
	Base struct containing information about effect. It is not applied directly to targets.
*/
USTRUCT(BlueprintType)
struct FGAEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	class UGAEffectSpecification* EffectSpec;
	/*
		Name of the effect.

		Name of effect is derived from object (Causer) which apply this effect, be it ability, character,
		projectile, weapon etc.

		If you have blueprint ability called Fireball, name of effect will be Fireball_C.

		It's used internally to track effects, from different abilities, weapons, etc., as effect on it's own, does
		not have any real meaning.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectName EffectName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectPolicy Policy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectDuration EffectDuration;

	/*
		Context of effect containing info about target, instigator, causer etc.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Base")
		FGAEffectContext Context;
	/*
		Select calculation type, which will be used, for calculating final value of
		this effect.
	*/
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UGACalculation> CalculationType;
	/*
		If you don't want  effect to spawn Cue, you can simply leave it empty.
	*/
	UPROPERTY(EditAnywhere, Category = "Cues")
		TSubclassOf<class AGAEffectCue> EffectCue;

	UPROPERTY(EditAnywhere, Category = "Cues")
		TSubclassOf<class UGAUIData> UIData;

	FGAEffectSpec()
	{};
	FGAEffectSpec(class UGAEffectSpecification* EffectSpecIn, const FGAEffectContext& ContextIn);


	TArray<FGAAttributeData> GetInitialAttribute();
	TArray<FGAAttributeData> GetDurationAttribute();
	TArray<FGAAttributeData> GetPeriodAttribute();
	TArray<FGAAttributeData> GetRemovedAttribute();
	TArray<FGAAttributeData> GetExpiredAttribute();

	TArray<FGAEffectModifierSpec> GetEffectModifiers();
};


/*
Contains definitions for specific modifer we want to override;
*/
USTRUCT(BlueprintType)
struct FGAModifierOverride
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	Attribute associated with modifier.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGAAttribute Attribute;
	/*
	New modifier we want to apply.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGAAttributeModifier Modifier;
};

USTRUCT()
struct FGAEffectInstant
{
	GENERATED_USTRUCT_BODY()
public:
	FGAEffectInstant()
	{}
	FGAEffectInstant(FGAEffectSpec& SpecIn, const FGAEffectContext& ContextIn);
	FGAEffectContext Context;
	void OnApplied();
	TArray<FGAAttributeData> InitialAttribute;

	FGameplayTagContainer OwnedTags;
};

/*
	Should we:
	1. Cache off, effect specs, as shared pointers somewhere ?
	We could use map with EffectName,TSharedPtr<Spec>, and instead of creating new spec every time
	check map first, find existing spec, add WeakPtr to active effect, and not copy bunch of stuff,
	which is used only occasionaly.
*/
struct GAMEATTRIBUTES_API FGAActiveDuration : public TSharedFromThis<FGAActiveDuration>
{
	friend struct FGAActiveEffectContainer;
	/** Pointer to effect asset, from which we can derive information about effect. */
	TWeakObjectPtr<class UGAEffectSpecification> EffectSpec;

	/* Current handle of this effect. */
	FGAEffectHandle MyHandle;

	/* Name of effect derived from EffectSpec. */
	FGAEffectName EffectName;

	/* Aggregation type derived from EffectSpec. */
	EGAEffectAggregation AggregationType;

	EGAEffectStacking Stacking;

	FGAEffectContext Context;

	TSubclassOf<class UGACalculation> CalculationType;
	TArray<FGAAttributeData> InitialModifiers;
	TArray<FGAAttributeData> DurationModifiers;
	TArray<FGAAttributeData> PeriodicModifiers;
	/* Attribute changes applied when effect is removed externally. */
	TArray<FGAAttributeData> RemovedAttribute;
	/* Attribute changes applied when effect naturally expires. */
	TArray<FGAAttributeData> ExpiredAttribute;
	/* Duration of effect. */
	float Duration;
	/* Time interval between periods. */
	float Period;

	float StackedDuration;
	/* How many effects are stacked. */
	int32 CurrentStack;

	FGameplayTagContainer RequiredTags;

	//add captured attributes from Instigator/Source.
	/*
	Normally attributes on each tick, are checked from instigator, but when Instigator dies (is destroyed)
	we should revert to checking cached relevelant attributes directly in effect.
	*/

	/*
		Tags describing this effect.
	*/
	FGameplayTagContainer OwnedTags;
	

	void RemoveDurationAttribute();
		
public:
	/* 
		Called when effect is applied. 
		Applies InitialAttribute and DurationAttribute.
	*/
	void OnApplied();
	/* Called on period of this effect. */
	void OnPeriod();
	/* Called when effect externally removed. */
	void OnRemoved();
	/* Called when effect naturally expires. */
	void OnEnded();
protected:
	FTimerHandle PeriodTimerHandle;
	FTimerHandle DurationTimerHandle;

public:
	/*
		Return true if OtherIn is higher than, any existing mod.
	*/
	bool ComparePeriodModifiers(const FGAAttributeData& OtherIn);
	void StackDuration(float NewDuration);
	void ActivateEffect();
	void FinishEffect();
	FGAActiveDuration()
	{}

	FGAActiveDuration(const FGAEffectContext& ContextIn)
		//:	Context(ContextIn)
	{
		Context = ContextIn;
	}
	FGAActiveDuration(const FGAEffectContext& ContextIn, FGAEffectSpec& SpecIn,
		const FGAEffectHandle& HandleIn);

	~FGAActiveDuration();
};

/*
This struct contains information needed for effect, to modify attributes
on other effects,
*/
USTRUCT(BlueprintType)
struct FGAEffectModifier
{
	GENERATED_USTRUCT_BODY()
public:
	FGAModifierStack IncomingStack;
	FGAModifierStack OutgoingStack;

	TMap<FGAEffectHandle, TArray<FGAModifier>> IncomingModifiers;
	TMap<FGAEffectHandle, TArray<FGAModifier>> OutgoingModifiers;

	void RemoveMod(const FGAEffectHandle& HandleIn);
	void CalculateIncomingBonus();
	void CalculateOutgoingBonus();
	void AddBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle);
	void AddIncomingBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle);
	void AddOutgoingBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle);
	void RemoveBonus(const FGAEffectHandle& Handle);
};

/*
Group modifiers, for this attribute and this Tag in single struct.
*/
USTRUCT(BlueprintType)
struct FGEffectModifierGroup
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	Attribute other effect modify, which we will modify on this effect.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	/*
	Only one tag, per one modifier, so we have EXACT match.
	If you want more generic modifier, you just need to use Tag higher in hierarchy.
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTag AttributeTag;
	UPROPERTY(EditAnywhere)
		TArray<FGAEffectModifier> Modifiers;

};
USTRUCT()
struct FGACalculatedEffectModifier
{
	GENERATED_USTRUCT_BODY()
};
USTRUCT()
struct GAMEATTRIBUTES_API FGAEffectModifierContainer
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Map of all modifiers.
	*/
	TMap<FGameplayTag, FGAEffectModifier> Modifiers;

	TArray<FGAEffectModifierSpec> ModifiersSpecs;

	void RemoveModifier(const FGameplayTagContainer& TagsIn, const FGAEffectHandle& HandleIn);
	void AddModifier(const FGAEffectModifierSpec& ModSpec, const FGameplayTagContainer& Tags, 
		const FGAEffectHandle HandleIn,
		TSharedPtr<FGAActiveDuration> EffectPtr);

	void CheckIfStronger(const FGameplayTagContainer& TagsIn, TArray<FGAEffectModifierSpec>& ModSpecs);

	FGAModifierStack GetIncomingModifierStack(const FGAAttributeData& DataIn);
	FGAModifierStack GetOutgoingModifierStack(const FGAAttributeData& TagIn);
};

USTRUCT()
struct FGAEffectTagHandle
{
	GENERATED_USTRUCT_BODY()
public:
	FGAEffectName EffectName;
	FGAEffectHandle Handle;

	FGAEffectTagHandle()
	{}
	FGAEffectTagHandle(const FGAEffectName& EffectNameIn,
		const FGAEffectHandle& HandleIn)
		: EffectName(EffectNameIn),
		Handle(HandleIn)
	{};

	inline const bool operator==(const FGAEffectTagHandle& OtherIn) const
	{
		return OtherIn.EffectName == EffectName
			&& OtherIn.Handle == Handle;
	};
};

/*
	Group effects, per Instigator.
*/
USTRUCT()
struct FGAInstigatorEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		We can assume that effects, with the same tag, are of the same type.
	*/
	TMap<FGAEffectName, TArray<FGAEffectHandle>> EffectsByName;
	TArray<FGAEffectTagHandle> Effects;
};

/*
	Active effect is struct made for replication and UI.
	It's very simple, and contains only rudimentary information about effect.
	Stacks, remaining duration, handle to effect.

	It's also not really synced with server. That means, server have total authorative controll
	over effects, and what is in this struct is approximation. 
	Only begin and end of effect are synced by server (wehther end is natural or not is irrelevelant).

	Does not contain information, about what is modified and how.

	What happen in between is entirely up to client.

	This struct could be predictively applied to target, but as of now it's not yet supported.
*/
USTRUCT(BlueprintType)
struct FGAActiveEffect // : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FGAEffectHandle MyHandle;
	UPROPERTY()
		float Duration;
	UPROPERTY()
		float Period;
	UPROPERTY()
		FGAEffectContext Context;
	UPROPERTY()
		float WorldStartTime;

	UPROPERTY()
		TSubclassOf<class UGAUIData> UIDataClass;

	UPROPERTY()
		TSubclassOf<class AGAEffectCue> CueClass;
	/* Indicates if effect has already been actiated. */
	UPROPERTY()
		uint32 bIsActivated : 1;
	/*
	Weak reference to cue associated with this effect.
	*/
	UPROPERTY(NotReplicated)
		TWeakObjectPtr<class AGAEffectCue> EffectCue;
public:
	float GetRemainingDuration(float CurrentWorldTime);


public:
	FGAActiveEffect()
	{};

	FGAActiveEffect(const FGAEffectHandle& HandleIn, FGAEffectSpec& SpecIn, float StartTimeIn);

	FGAActiveEffect(const FGAEffectHandle& HandleIn, FGAEffectSpec& SpecIn, float StartTimeIn,
					TSubclassOf<class UGAUIData> UIDataIn, TSubclassOf<class AGAEffectCue> CueIn);
};

USTRUCT()
struct FGACachedEffectSpecContainer
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		It's easy to find based on effect name, since it will be either unique to spec
		or simply shared between multiple specs.
	*/
	TMap<FGAEffectName, TSharedPtr<FGAEffectSpec>> CachedSpecs;
};

/*
	Notes:

	Implement fast serialization. Latter, when basics works.
*/
USTRUCT()
struct FGAActiveEffectContainer// : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Map of all Active effects.
		Store as shared pointers, so we can have some basic polymorphism.

		Not, extremly important and might change later!
	*/
	TMap<FGAEffectHandle, TSharedPtr<FGAActiveDuration>> ActiveEffects;
	
	/*
		List of active effects, for replication and UI.
	*/
	UPROPERTY()
		TArray<FGAActiveEffect> RepActiveEffects;

	/*
		Group effects, by instigator who applied to it.
		Each instigator get separate pool, against which stacking rules are checked.
	*/
	TMap<TWeakObjectPtr<class UGAAttributeComponent>, FGAInstigatorEffectContainer> InstigatorEffects;

	/*
		Effects aggregated by Target.
	*/
	TMap<FGAEffectName, TArray<FGAEffectHandle>> MyEffects;

	FGAEffectModifierContainer ModifierContainer;

	FGAActiveEffectContainer()
	{}
	

	/*
		Both functions are only for internal use!
		These are not safe to use outside of EffectContainer.
	*/
	/*
		Generic function, which will remove effect, given handle.
		Does not perform any checks.
	*/
	void RemoveActiveEffect(const FGAEffectHandle& HandleIn);
public:
	FGAEffectHandle ApplyEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Ctx);
	FGAEffectHandle ApplyEffect(TSubclassOf<class UGAEffectSpecification> SpecIn, 
		const FGAEffectContext& Ctx, const FName& EffectName);
	void Clean();
protected:
	/**
	 *	Generic function, which will add any effect.
	 *	It does not perform any checks.
	 */
	FGAEffectHandle AddActiveEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

	/* Handles instant effects. */
	FGAEffectHandle HandleInstantEffect(FGAEffectInstant& SpecIn, const FGAEffectContext& Ctx);
	/* Handles effects, which have set duration. */
	FGAEffectHandle HandleDurationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/**
	 *	Instigator aggregated effects, are aggregated per instigator (AttributeComponent).
	 *	These effects do not interact with effects, from other instigators, 
	 *	and are checked only against other effects, from the same instigator.
	 */
	FGAEffectHandle HandleInstigatorAggregationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/** 
	 *	This is very complicated case and it's not done yet.
	 *	Basically, it find and compare modifiers, from effects, and replace the weaker ones,
	 *	with stronger ones.
	 */
	FGAEffectHandle	HandleInstigatorEffectStrongerOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/* Finds effect of the same type, remove it, and apply new effect. */
	FGAEffectHandle HandleInstigatorEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/* Adds duration from new effect, to the old effect of the same type. */
	FGAEffectHandle	HandleInstigatorEffectDuration(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/* will sum all modifiers of the same type, from incoming effect, will not refresh duration. */
	FGAEffectHandle	HandleInstigatorEffectDIntensity(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/* Simply add new effect. Does not do, any checks or removals. */
	FGAEffectHandle	HandleInstigatorEffectAdd(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

	/**
	 *	Target aggregated effects, are aggregated on per target basis regardless of instigator.
	 *	Target effects are checked only against other effects, from the same target.
	 */
	FGAEffectHandle HandleTargetAggregationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/**
	 *	This is very complicated case and it's not done yet.
	 *	Basically, it find and compare modifiers, from effects, and replace the weaker ones,
	 *	with stronger ones.
	 */
	FGAEffectHandle	HandleTargetEffectStrongerOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/* Finds effect of the same type, remove it, and apply new effect. */
	FGAEffectHandle HandleTargetEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/* Adds duration from new effect, to the old effect of the same type. */
	FGAEffectHandle HandleTargetEffectDuration(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/* will sum all modifiers of the same type, from incoming effect, will not refresh duration. */
	FGAEffectHandle HandleTargetEffectIntensity(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/* Simply add new effect. Does not do, any checks or removals. */
	FGAEffectHandle HandleTargetEffectAdd(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

	/* Remove effect aggregated by target. */
	void RemoveTargetAggregation(TSharedPtr<FGAActiveDuration> EffectIn);
	/* Remove effect aggregated by instigator. */
	void RemoveInstigatorAggregation(TSharedPtr<FGAActiveDuration> EffectIn);
};


