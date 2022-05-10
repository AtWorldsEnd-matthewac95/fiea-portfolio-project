#include "gamestatemachine.h"
#include "battlerdecision.h"

namespace AWE {
    GameStateMachine::GameStateMachine(GameXLOStorage& xlo, GameSceneTransitioner& scenes, GameBattleInfo& battle)
            : GameState()
            , _xlo(&xlo)
            , _scenes(&scenes)
            , _battle(&battle)
            , _isSceneInitialized(false)
            , _isBattleInitialized(false)
            , _isBattleConfigured(false)
            , _isBattleLostConfigured(false)
            , _isLevelupInitialized(false)
            , _isLevelupConfigured(false)
            , _sfmls(nullptr)
            , _index(0) {
        _map.insert(std::make_pair(GameStateType::SCENE_FADEIN, std::make_unique<GameState_Scene_FadeIn>(GameState_Scene_FadeIn())));
        _map.insert(std::make_pair(GameStateType::SCENE_FADEOUT, std::make_unique<GameState_Scene_FadeOut>(GameState_Scene_FadeOut())));
        _map.insert(std::make_pair(GameStateType::SCENE_MUSIC, std::make_unique<GameState_Scene_Music>(GameState_Scene_Music())));
        _map.insert(std::make_pair(GameStateType::MACHINESCENE_TRANSITION, std::make_unique<GameState_MachineScene_Transition>(GameState_MachineScene_Transition(*this))));
        _map.insert(std::make_pair(GameStateType::BATTLE_AISKILLSELECT, std::make_unique<GameState_Battle_AISkillSelect>(GameState_Battle_AISkillSelect())));
        _map.insert(std::make_pair(GameStateType::BATTLE_DAMAGECALCULATION, std::make_unique<GameState_Battle_DamageCalculation>(GameState_Battle_DamageCalculation())));
        _map.insert(std::make_pair(GameStateType::BATTLE_MONOLOGUE, std::make_unique<GameState_Battle_Monologue>(GameState_Battle_Monologue())));
        _map.insert(std::make_pair(GameStateType::BATTLE_PLAYERLOST, std::make_unique<GameState_Battle_PlayerLost>(GameState_Battle_PlayerLost())));
        _map.insert(std::make_pair(GameStateType::BATTLE_PLAYERSKILLSELECT, std::make_unique<GameState_Battle_PlayerSkillSelect>(GameState_Battle_PlayerSkillSelect())));
        _map.insert(std::make_pair(GameStateType::BATTLE_PLAYERWON, std::make_unique<GameState_Battle_PlayerWon>(GameState_Battle_PlayerWon())));
        _map.insert(std::make_pair(GameStateType::BATTLE_SKILLANIMATION, std::make_unique<GameState_Battle_SkillAnimation>(GameState_Battle_SkillAnimation())));
        _map.insert(std::make_pair(GameStateType::BATTLE_SKILLNAME, std::make_unique<GameState_Battle_SkillName>(GameState_Battle_SkillName())));
        _map.insert(std::make_pair(GameStateType::MACHINEBATTLE_BATTLELOOP, std::make_unique<GameState_MachineBattle_BattleLoop>(GameState_MachineBattle_BattleLoop(*this))));
        _map.insert(std::make_pair(GameStateType::MACHINEBATTLE_PLAYERLOOP, std::make_unique<GameState_MachineBattle_PlayerLoop>(GameState_MachineBattle_PlayerLoop(*this))));
        _map.insert(std::make_pair(GameStateType::MACHINEBATTLE_SKILLINIT, std::make_unique<GameState_MachineBattle_SkillInit>(GameState_MachineBattle_SkillInit(*this))));
        _map.insert(std::make_pair(GameStateType::MACHINEBATTLE_SKILLLOOP, std::make_unique<GameState_MachineBattle_SkillLoop>(GameState_MachineBattle_SkillLoop(*this))));
        _map.insert(std::make_pair(GameStateType::LEVELUP_LOADCHARACTERS, std::make_unique<GameState_LevelUp_LoadCharacters>(GameState_LevelUp_LoadCharacters())));
        _map.insert(std::make_pair(GameStateType::LEVELUP_SELECT, std::make_unique<GameState_LevelUp_Select>(GameState_LevelUp_Select())));
        _map.insert(std::make_pair(GameStateType::MACHINELEVELUP_SETSELECT, std::make_unique<GameState_MachineLevelUp_SetSelect>(GameState_MachineLevelUp_SetSelect(*this))));
        _map.insert(std::make_pair(GameStateType::MACHINELEVELUP_ADVANCE, std::make_unique<GameState_MachineLevelUp_Advance>(GameState_MachineLevelUp_Advance(*this))));
        _map.insert(std::make_pair(GameStateType::CLOSE, std::make_unique<GameState_Close>(GameState_Close())));
        _map.insert(std::make_pair(GameStateType::STOP, std::make_unique<GameState>(GameState())));
    }

    GameStateMachine::GameStateMachine(GameXLOStorage& xlo, GameSceneTransitioner& scenes, GameBattleInfo& battle, GameSFMLStorage& sfmls, sf::Sound& skillSound) : GameStateMachine(xlo, scenes, battle) {
        InitializeScene(sfmls);
        InitializeBattle(sfmls, skillSound);
        InitializeLevelup(sfmls);
    }

    bool GameStateMachine::ResetBack(GameStateType to) {
        GameState* curr = current();

        if (!curr) {
            return false;
        }

        curr->ResetSteps();

        while (_index > 0 && curr->stateType() != to) {
            _index--;
            curr = current();

            if (!curr) {
                return false;
            }

            curr->ResetSteps();
        }

        return curr && curr->stateType() == to;
    }

    bool GameStateMachine::isSceneInitialized() const { return _isSceneInitialized; }
    bool GameStateMachine::isBattleInitialized() const { return _isBattleInitialized; }
    bool GameStateMachine::isBattleConfigured() const { return _isBattleConfigured; }
    bool GameStateMachine::isLevelupInitialized() const { return _isLevelupInitialized; }
    bool GameStateMachine::isLevelupConfigured() const { return _isLevelupConfigured; }
    GameState* GameStateMachine::current() { return _states.empty() || _index >= _states.size() ? nullptr : _states.at(_index); }

    template <typename TGameState> TGameState* GameStateMachine::GetState(GameStateType statetype) {
        TGameState* state = nullptr;

        auto found = _map.find(statetype);
        if (found != _map.end()) {
            state = dynamic_cast<TGameState*>(found->second.get());
        }

        return state;
    }

    void GameStateMachine::ConfigureForBeginning() {
        _states.clear();
        _states.push_back(_map.at(GameStateType::MACHINESCENE_TRANSITION).get());
        _index = 0;
    }

    bool GameStateMachine::InitializeScene(GameSFMLStorage& sfmls) {
        if (_isSceneInitialized) {
            return true;
        }

        if (!_sfmls) {
            _sfmls = &sfmls;
        }

        auto fadeInState = GetState<GameState_Scene_FadeIn>(GameStateType::SCENE_FADEIN);
        fadeInState->fadebox(*_sfmls->fadebox());
        fadeInState->speed(16);

        auto fadeOutState = GetState<GameState_Scene_FadeOut>(GameStateType::SCENE_FADEOUT);
        fadeOutState->fadebox(*_sfmls->fadebox());
        fadeOutState->speed(16);

        auto musicState = GetState<GameState_Scene_Music>(GameStateType::SCENE_MUSIC);
        musicState->scenes(*_scenes);
        musicState->sfmls(*_sfmls);

        _isSceneInitialized = true;
        return _isSceneInitialized;
    }

    bool GameStateMachine::InitializeBattle(GameSFMLStorage& sfmls, sf::Sound& skillSound) {
        if (_isBattleInitialized) {
            return true;
        }

        if (!_sfmls) {
            _sfmls = &sfmls;
        }

        auto monologueState = GetState<GameState_Battle_Monologue>(GameStateType::BATTLE_MONOLOGUE);
        monologueState->prompt(_sfmls->textboxes()->at(GameTextboxType::GENERIC));

        auto aiSkillState = GetState<GameState_Battle_AISkillSelect>(GameStateType::BATTLE_AISKILLSELECT);
        aiSkillState->battle(*_battle);

        auto playerSkillState = GetState<GameState_Battle_PlayerSkillSelect>(GameStateType::BATTLE_PLAYERSKILLSELECT);
        playerSkillState->characterIndex(0);
        playerSkillState->battle(*_battle);
        _sfmls->battleMenu()->characters(_battle->characters());
        playerSkillState->menu(*_sfmls->battleMenu());
        playerSkillState->selectSound(*_sfmls->GetSound(GameSoundType::SAVE));

        auto skillNameState = GetState<GameState_Battle_SkillName>(GameStateType::BATTLE_SKILLNAME);
        skillNameState->textbox(_sfmls->textboxes()->at(GameTextboxType::SKILL));

        auto skillAnimState = GetState<GameState_Battle_SkillAnimation>(GameStateType::BATTLE_SKILLANIMATION);
        skillAnimState->skillsprite(*_sfmls->GetSprite(GameTextureType::SKILL));
        skillAnimState->sound(skillSound);

        auto damageState = GetState<GameState_Battle_DamageCalculation>(GameStateType::BATTLE_DAMAGECALCULATION);
        damageState->calc(_battle->damagecalc());
        damageState->damagetext(_sfmls->textboxes()->at(GameTextboxType::DAMAGE));
        damageState->skilltext(_sfmls->textboxes()->at(GameTextboxType::SKILL));
        damageState->prompttext(_sfmls->textboxes()->at(GameTextboxType::GENERIC));

        auto wonState = GetState<GameState_Battle_PlayerWon>(GameStateType::BATTLE_PLAYERWON);
        wonState->enemySprite(*_sfmls->GetSprite(GameTextureType::ENEMY));
        wonState->defeatSound(*_sfmls->GetSound(GameSoundType::PEPSI));
        wonState->fadeColor(sf::Color::Red);
        wonState->speed(16u);

        auto lostState = GetState<GameState_Battle_PlayerLost>(GameStateType::BATTLE_PLAYERLOST);
        lostState->prompt(_sfmls->textboxes()->at(GameTextboxType::GENERIC));

        _isBattleConfigured = false;
        _isBattleLostConfigured = false;
        _isBattleInitialized = true;
        return _isBattleInitialized;
    }

    bool GameStateMachine::ConfigureForBattle() {
        if (!_isSceneInitialized || !_isBattleInitialized) {
            return false;
        }

        _states.clear();
        _states.push_back(_map.at(GameStateType::SCENE_FADEIN).get());
        _states.push_back(_map.at(GameStateType::SCENE_MUSIC).get());
        _states.push_back(_map.at(GameStateType::BATTLE_MONOLOGUE).get());
        _states.push_back(_map.at(GameStateType::BATTLE_AISKILLSELECT).get());
        _states.push_back(_map.at(GameStateType::BATTLE_PLAYERSKILLSELECT).get());
        _states.push_back(_map.at(GameStateType::MACHINEBATTLE_PLAYERLOOP).get());
        _states.push_back(_map.at(GameStateType::MACHINEBATTLE_SKILLINIT).get());
        _states.push_back(_map.at(GameStateType::BATTLE_SKILLNAME).get());
        _states.push_back(_map.at(GameStateType::BATTLE_SKILLANIMATION).get());
        _states.push_back(_map.at(GameStateType::BATTLE_DAMAGECALCULATION).get());
        _states.push_back(_map.at(GameStateType::MACHINEBATTLE_SKILLLOOP).get());
        _states.push_back(_map.at(GameStateType::MACHINEBATTLE_BATTLELOOP).get());
        _states.push_back(_map.at(GameStateType::BATTLE_PLAYERWON).get());
        _states.push_back(_map.at(GameStateType::SCENE_FADEOUT).get());
        _states.push_back(_map.at(GameStateType::MACHINESCENE_TRANSITION).get());
        _states.push_back(_map.at(GameStateType::CLOSE).get());
        _states.push_back(_map.at(GameStateType::STOP).get());

        _index = 0;
        _isBattleConfigured = true;
        _isBattleLostConfigured = false;
        _isLevelupConfigured = false;

        return _isBattleConfigured;
    }

    bool GameStateMachine::ConfigureForBattleLost() {
        if (!_isSceneInitialized || !_isBattleInitialized) {
            return false;
        }

        _states.clear();
        _states.push_back(_map.at(GameStateType::BATTLE_PLAYERLOST).get());
        _states.push_back(_map.at(GameStateType::SCENE_FADEOUT).get());
        _states.push_back(_map.at(GameStateType::CLOSE).get());
        _states.push_back(_map.at(GameStateType::STOP).get());

        _index = 0;
        _isBattleConfigured = false;
        _isBattleLostConfigured = true;
        _isLevelupConfigured = false;

        return _isBattleConfigured;
    }

    bool GameStateMachine::InitializeLevelup(GameSFMLStorage& sfmls) {
        if (_isLevelupInitialized) {
            return true;
        }

        if (!_sfmls) {
            _sfmls = &sfmls;
        }

        auto charloadState = GetState<GameState_LevelUp_LoadCharacters>(GameStateType::LEVELUP_LOADCHARACTERS);
        charloadState->xlo(*_xlo);

        auto selectState = GetState<GameState_LevelUp_Select>(GameStateType::LEVELUP_SELECT);
        selectState->cursorSound(*_sfmls->GetSound(GameSoundType::CURSOR));
        selectState->selectSound(*_sfmls->GetSound(GameSoundType::SAVE));

        _isLevelupConfigured = false;
        _isLevelupInitialized = true;
        return _isLevelupInitialized;
    }

    bool GameStateMachine::ConfigureForLevelup() {
        if (!_isSceneInitialized || !_isLevelupInitialized) {
            return false;
        }

        _states.clear();
        _states.push_back(_map.at(GameStateType::SCENE_FADEIN).get());
        _states.push_back(_map.at(GameStateType::SCENE_MUSIC).get());
        _states.push_back(_map.at(GameStateType::LEVELUP_LOADCHARACTERS).get());
        _states.push_back(_map.at(GameStateType::MACHINELEVELUP_SETSELECT).get());
        _states.push_back(_map.at(GameStateType::LEVELUP_SELECT).get());
        _states.push_back(_map.at(GameStateType::MACHINELEVELUP_ADVANCE).get());
        _states.push_back(_map.at(GameStateType::SCENE_FADEOUT).get());
        _states.push_back(_map.at(GameStateType::MACHINESCENE_TRANSITION).get());
        _states.push_back(_map.at(GameStateType::STOP).get());

        _index = 0;
        _isBattleConfigured = false;
        _isBattleLostConfigured = false;
        _isLevelupConfigured = true;

        return _isLevelupConfigured;
    }

    bool GameStateMachine::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_isBattleConfigured && !_isLevelupConfigured) {
            ConfigureForBeginning();
        }

        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameStateMachine::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        GameState* curr = current();

        if (!curr->Update()) {
            return false;
        }

        if (!curr->isDone()) {
            return true;
        }

        _index++;
        if (_index >= _states.size()) {
            _step = GameStateStep::ENDING;
        }

        return true;
    }

    bool GameStateMachine::ResetSteps() {
        _index = _states.size() - 1;
        return ResetBack((*_states.begin())->stateType());
    }


    /* -- Scene -- */


    /* Transition */

    GameState_MachineScene_Transition::GameState_MachineScene_Transition(GameStateMachine& parent) : GameState(), _parent(&parent) {}

    bool GameState_MachineScene_Transition::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current || current->stateType() != stateType()) {
            return false;
        }

        GameSceneTransitioner* scenes = _parent->_scenes;

        if (!scenes) {
            return false;
        }

        if (!scenes->GoToNextScene()) {
            _step = GameStateStep::DONE;
            return true;
        }

        GameSceneInfo* scene = scenes->GetCurrent();

        if (!scene) {
            return false;
        }

        bool result = false;

        switch (scene->background()) {

        case GameTextureType::WOODS:
            _parent->ResetSteps();
            result = _parent->ConfigureForBattle();
            break;

        case GameTextureType::TEMPLE:
            _parent->ResetSteps();
            _parent->_sfmls->GetSprite(GameTextureType::ENEMY)->isVisible(false);
            result = _parent->ConfigureForLevelup();
            break;

        case GameTextureType::FIRETOWN:
            _parent->ResetSteps();
            _parent->_battle->GoToNextEnemy(_parent->_xlo->battlers());
            _parent->_sfmls->GetSprite(GameTextureType::ENEMY)->textureIndex(_parent->_battle->enemy()->textureIndex());
            _parent->_battle->RefreshCharacters(*_parent->_xlo);
            _parent->GetState<GameState_Battle_Monologue>(GameStateType::BATTLE_MONOLOGUE)->thomas(true);
            result = _parent->ConfigureForBattle();
            break;

        }

        if (!result) {
            _step = GameStateStep::DONE;
        }

        return true;
    }


    /* -- Battle -- */


    /* Player Loop */

    GameState_MachineBattle_PlayerLoop::GameState_MachineBattle_PlayerLoop(GameStateMachine& parent) : GameState(), _parent(&parent) {}

    void GameState_MachineBattle_PlayerLoop::ConductReset() {
        if (!_parent->_isBattleInitialized || !_parent->_isBattleConfigured) {
            return;
        }

        _parent->ResetBack(GameStateType::BATTLE_PLAYERSKILLSELECT);
    }

    bool GameState_MachineBattle_PlayerLoop::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current || !_parent->_isBattleInitialized || !_parent->_isBattleConfigured) {
            return false;
        }

        auto playerSkillState = _parent->GetState<GameState_Battle_PlayerSkillSelect>(GameStateType::BATTLE_PLAYERSKILLSELECT);
        if (!playerSkillState || !playerSkillState->isDone()) {
            return false;
        }

        int newindex = playerSkillState->characterIndex() + 1;

        if (newindex >= _parent->_battle->characters().size()) {
            _step = GameStateStep::DONE;
            return true;
        }

        playerSkillState->characterIndex(newindex);

        if (current->stateType() == stateType()) {
            ConductReset();
        } else {
            _step = GameStateStep::PROCESSING;
        }

        return true;
    }

    bool GameState_MachineBattle_PlayerLoop::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current || !_parent->_isBattleInitialized || !_parent->_isBattleConfigured || !_parent->_map.at(GameStateType::BATTLE_PLAYERSKILLSELECT)->isDone()) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        if (current->stateType() == stateType()) {
            ConductReset();
        }
        return true;
    }


    /* Skill Init */

    GameState_MachineBattle_SkillInit::GameState_MachineBattle_SkillInit(GameStateMachine& parent) : GameState(), _parent(&parent) {}

    bool GameState_MachineBattle_SkillInit::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_parent->_isBattleInitialized || !_parent->_isBattleConfigured) {
            return false;
        }

        auto skillNameState = _parent->GetState<GameState_Battle_SkillName>(GameStateType::BATTLE_SKILLNAME);
        if (!skillNameState) {
            return false;
        }

        auto skillAnimState = _parent->GetState<GameState_Battle_SkillAnimation>(GameStateType::BATTLE_SKILLANIMATION);
        if (!skillAnimState) {
            return false;
        }

        auto calcState = _parent->GetState<GameState_Battle_DamageCalculation>(GameStateType::BATTLE_DAMAGECALCULATION);
        if (!calcState) {
            return false;
        }

        BattlerDecisionOrdering::const_iterator decisitr;
        if (!CleanTopBattlerDecisionOrdering(_parent->_battle->decisions(), decisitr)) {
            return false;
        }

        const BattlerDecision& decision = *decisitr->second.begin();
        AWESprite& targetsprite = _parent->_sfmls->sprites()->at(decision.target()->textureType());

        skillNameState->skill(decision.skill());
        skillAnimState->skillsprite()->textureIndex(decision.skill()->textureIndex());
        skillAnimState->sound()->setBuffer(_parent->_sfmls->sounds().at(decision.skill()->soundFilename()));
        skillAnimState->targetsprite(targetsprite);
        calcState->decision(decision);
        calcState->targetsprite(targetsprite);

        _step = GameStateStep::DONE;
        return true;
    }


    /* Skill Loop */

    GameState_MachineBattle_SkillLoop::GameState_MachineBattle_SkillLoop(GameStateMachine& parent) : GameState(), _parent(&parent) {}

    void GameState_MachineBattle_SkillLoop::ConductReset() {
        if (!_parent->_isBattleInitialized || !_parent->_isBattleConfigured) {
            return;
        }

        _parent->ResetBack(GameStateType::MACHINEBATTLE_SKILLINIT);
    }

    bool GameState_MachineBattle_SkillLoop::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current
            || !_parent->_isBattleInitialized
            || !_parent->_isBattleConfigured
            || !_parent->_map.at(GameStateType::BATTLE_SKILLNAME)->isDone()
            || !_parent->_map.at(GameStateType::BATTLE_SKILLANIMATION)->isDone()
        ) {
            return false;
        }

        auto calcState = _parent->GetState<GameState_Battle_DamageCalculation>(GameStateType::BATTLE_DAMAGECALCULATION);
        if (!calcState || !calcState->isDone()) {
            return false;
        }

        auto removed = RemoveBattlerDecision(*_parent->_battle->decisions(), calcState->decision()->source());
        if (removed) {
            BattlerInstance_shptr target = removed->target();
            auto hp = target->hp();
            auto result = calcState->result()->final();
            bool defeat = result >= hp;
            target->hp(defeat ? 0 : (hp - result));

            if (defeat) {
                RemoveBattlerDecision(*_parent->_battle->decisions(), target);

                if (target->isCharacter()) {
                    _parent->_sfmls->sprites()->at(target->textureType()).textureIndex(1);
                }
            }
        }

        if (CleanTopBattlerDecisionOrdering(_parent->_battle->decisions())) {
            if (current->stateType() == stateType()) {
                ConductReset();
            } else {
                _step = GameStateStep::PROCESSING;
            }
        } else {
            _step = GameStateStep::DONE;
        }

        return true;
    }

    bool GameState_MachineBattle_SkillLoop::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current
            || !_parent->_isBattleInitialized
            || !_parent->_isBattleConfigured
            || !_parent->_map.at(GameStateType::BATTLE_SKILLNAME)->isDone()
            || !_parent->_map.at(GameStateType::BATTLE_SKILLANIMATION)->isDone()
            || !_parent->_map.at(GameStateType::BATTLE_DAMAGECALCULATION)->isDone()
        ) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        if (current->stateType() == stateType()) {
            ConductReset();
        }
        return true;
    }


    /* Battle Loop */

    GameState_MachineBattle_BattleLoop::GameState_MachineBattle_BattleLoop(GameStateMachine& parent) : GameState(), _parent(&parent), _battleresult(false) {}

    void GameState_MachineBattle_BattleLoop::ConductReset() {
        if (!_parent->_isBattleInitialized || !_parent->_isBattleConfigured) {
            return;
        }

        _parent->GetState<GameState_Battle_PlayerSkillSelect>(GameStateType::BATTLE_PLAYERSKILLSELECT)->characterIndex(0);
        _parent->ResetBack(GameStateType::BATTLE_AISKILLSELECT);
    }

    bool GameState_MachineBattle_BattleLoop::ResetSteps() { _battleresult = false; _step = GameStateStep::BEGINNING; return true; }

    bool GameState_MachineBattle_BattleLoop::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current || !_parent->_isBattleInitialized || !_parent->_isBattleConfigured) {
            return false;
        }

        if (_parent->_battle->enemy()->hp() <= 0) {
            _battleresult = true;
            _step = GameStateStep::ENDING;
            return true;
        }

        _battleresult = false;
        const std::vector<BattlerInstance_shptr>& characters = _parent->_battle->characters();
        for (const BattlerInstance_shptr& character : characters) {
            _battleresult |= character->hp() > 0;
            if (_battleresult) {
                break;
            }
        }

        if (!_battleresult) {
            _parent->_sfmls->StopMusic();
            _parent->ConfigureForBattleLost();
            return true;
        }

        if (current->stateType() == stateType()) {
            ConductReset();
        } else {
            _step = GameStateStep::PROCESSING;
        }

        return true;
    }

    bool GameState_MachineBattle_BattleLoop::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current || !_parent->_isBattleInitialized || !_parent->_isBattleConfigured) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        if (current->stateType() == stateType()) {
            ConductReset();
        }
        return true;
    }

    bool GameState_MachineBattle_BattleLoop::End() {
        if (_step != GameStateStep::ENDING) {
            return true;
        }

        _parent->GetState<GameState_Battle_PlayerSkillSelect>(GameStateType::BATTLE_PLAYERSKILLSELECT)->characterIndex(0);
        _parent->_sfmls->GetSprite(GameTextureType::EPPLER)->textureIndex(0);
        _parent->_sfmls->GetSprite(GameTextureType::REMI)->textureIndex(0);
        _step = GameStateStep::DONE;
        return true;
    }


    /* -- Level Up -- */


    /* Set Select */

    GameState_MachineLevelUp_SetSelect::GameState_MachineLevelUp_SetSelect(GameStateMachine& parent) : GameState(), _parent(&parent) {}

    bool GameState_MachineLevelUp_SetSelect::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_parent) {
            return false;
        }

        auto charloadState = _parent->GetState<GameState_LevelUp_LoadCharacters>(GameStateType::LEVELUP_LOADCHARACTERS);
        if (!charloadState || !charloadState->isDone()) {
            return false;
        }

        auto top = charloadState->_characters.begin();
        AWESprite* menu = _parent->_sfmls->GetSprite(top->first);

        if (!menu) {
            return false;
        }

        auto selectState = _parent->GetState<GameState_LevelUp_Select>(GameStateType::LEVELUP_SELECT);
        if (!selectState) {
            return false;
        }

        selectState->menu(*menu);
        selectState->battler(top->second);
        charloadState->_characters.erase(top->first);
        _step = GameStateStep::DONE;
        return true;
    }


    /* Advance */

    GameState_MachineLevelUp_Advance::GameState_MachineLevelUp_Advance(GameStateMachine& parent) : GameState(), _parent(&parent) {}

    void GameState_MachineLevelUp_Advance::ConductReset() {
        if (!_parent->_isLevelupInitialized || !_parent->_isLevelupConfigured) {
            return;
        }

        _parent->ResetBack(GameStateType::MACHINELEVELUP_SETSELECT);
    }

    bool GameState_MachineLevelUp_Advance::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current || !_parent->_isLevelupInitialized || !_parent->_isLevelupConfigured) {
            return false;
        }

        auto charloadState = _parent->GetState<GameState_LevelUp_LoadCharacters>(GameStateType::LEVELUP_LOADCHARACTERS);
        if (!charloadState) {
            return false;
        }

        if (charloadState->characters().empty()) {
            _step = GameStateStep::DONE;
            return true;
        }

        if (current->stateType() == stateType()) {
            ConductReset();
        } else {
            _step = GameStateStep::PROCESSING;
        }

        return true;
    }

    bool GameState_MachineLevelUp_Advance::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        GameState* current = _parent->current();

        if (!current || !_parent->_isLevelupInitialized || !_parent->_isLevelupConfigured) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        if (current->stateType() == stateType()) {
            ConductReset();
        }
        return true;
    }
}
