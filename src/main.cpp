#include <Geode/Geode.hpp>
#include <chrono>

using namespace geode::prelude;

#include <Geode/modify/EditorUI.hpp>
class $modify(MyEditorUI, EditorUI) {

	struct Fields {
		int m_lastObjectCount = 0;
		CCLabelBMFont* m_timeLabel;
		CCNode* m_lengthContainer;
		bool m_enabled;
	};

	void checkForObjectChange(float dt) {
		auto fields = m_fields.self();

		int last = fields->m_lastObjectCount;
		int cur = m_editorLayer->m_objectCount;

		if (last != cur) {
			queueInMainThread([this, fields] {
				fields->m_timeLabel->setString(getTime().c_str());
			});
		}
	
		fields->m_lastObjectCount = m_editorLayer->m_objectCount;
    }

	float getLastObjectX() {
		float lastX = 0.0f;

		for (GameObject* object : CCArrayExt<GameObject*>(m_editorLayer->m_objects)) {
			float objectX = object->getRealPosition().x;
			if (objectX > lastX) {
				lastX = objectX;
			}
		}

		return lastX;
	}

	std::string getTime() {

		float x = getLastObjectX();
		CCPoint point = {x + 340, 0};
		int seconds = LevelTools::timeForPos(point, m_editorLayer->m_drawGridLayer->m_speedObjects, (int)m_editorLayer->m_levelSettings->m_startSpeed, 0, 0, 0, 0, 0, 0, 0);
		int timestamp = m_editorLayer->m_level->m_timestamp;
		float time = timestamp/240.0f;
		if (timestamp > 0 && seconds < time) {
			seconds = time;
		}

		auto duration = std::chrono::seconds(seconds);
    	auto formattedTime = std::chrono::hh_mm_ss(duration);

		std::string timeString;

		if (formattedTime.hours().count() > 0) {
			timeString = fmt::format("{}h {}m {}s", 
				formattedTime.hours().count(), 
				formattedTime.minutes().count(), 
				formattedTime.seconds().count());
		} else if (formattedTime.minutes().count() > 0) {
			timeString = fmt::format("{}m {}s", 
				formattedTime.minutes().count(), 
				formattedTime.seconds().count());
		} else {
			timeString = fmt::format("{}s", formattedTime.seconds().count());
		}

		return timeString;
	}

    void showUI(bool p0) {
		EditorUI::showUI(p0);
		auto fields = m_fields.self();
		if (fields->m_enabled) {
			fields->m_lengthContainer->setVisible(p0);
		}
	}


	bool init(LevelEditorLayer* lel) {
		if (!EditorUI::init(lel)) {
			return false;
		}
		auto fields = m_fields.self();
		fields->m_enabled = !(m_editorLayer->m_levelSettings->m_platformerMode || !Mod::get()->getSettingValue<bool>("is-enabled"));

		if (fields->m_enabled) {
			CCSize winSize = CCDirector::get()->getWinSize();
			CCNode* objectInfoLabel = getChildByID("object-info-label");

			fields->m_lengthContainer = CCNode::create();

			fields->m_lengthContainer->setPosition({objectInfoLabel->getPositionX() + 20, objectInfoLabel->getPositionY() - 10});
			fields->m_lengthContainer->setScale(0.5f);
			fields->m_lengthContainer->setID("length-container"_spr);
			objectInfoLabel->setPosition({objectInfoLabel->getPositionX(), objectInfoLabel->getPositionY() - 25});


			CCLabelBMFont* lengthLabel = CCLabelBMFont::create("Length", "bigFont.fnt");
			lengthLabel->setPositionY(8);
			lengthLabel->setScale(0.5f);
			lengthLabel->setID("length-label"_spr);

			fields->m_timeLabel = CCLabelBMFont::create("1s", "chatFont.fnt");
			fields->m_timeLabel->setPositionY(-8);
			fields->m_timeLabel->setID("time-label"_spr);

			fields->m_lengthContainer->addChild(lengthLabel);
			fields->m_lengthContainer->addChild(fields->m_timeLabel);

			addChild(fields->m_lengthContainer);
        	schedule(schedule_selector(MyEditorUI::checkForObjectChange));
		}

		return true;
	}
};