import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
// this toolbar is not efficient even a bit.
// and this whole toolbar(and operations related to) must change.

Rectangle {
    width: parent.width
    height: 80
    property alias tbar: tabbar
    RowLayout {
        x: 90
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - 100
        height: parent.height
        RowLayout {
            id: appinfo
            Label {
                id: appicon
                font.family: "fontello"
                text: stackView.tabBarNeeded ? "\uf111" : "\ue801"
                color: "#f56565"
                font.pixelSize: 15
            }
            Label {
                id: appname
                text: "Hacker News"
                font.pixelSize: 20
                font.bold: Font.Medium
            }
            MouseArea {
                anchors.fill: parent
                enabled: !stackView.tabBarNeeded
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (tabbar.currentIndex == -1) {
                        commentmodel.reset()
                    }
                    tabbar.currentIndex = 0
                    stackView.tabBarNeeded = true
                    stackView.pop()
                }
            }
        }
        ListView {
            id: tabbar
            visible: stackView.tabBarNeeded
            Layout.bottomMargin: 18
            Layout.leftMargin: 50
            Layout.fillWidth: true
            currentIndex: 0
            orientation: ListView.Horizontal
            interactive: false
            model: ["Top", "New", "Search", "Submit"]
            spacing: 10
            delegate: Item {
                width: 70
                height: 20
                RowLayout {
//                            Layout.leftMargin: 40
                    Label {
                        id: newicon
                        font.family: "fontello"
                        text: tabbar.currentIndex == index ? "\uf111" : "\uf10c"
                        color: "#f56565"
                        font.pixelSize: 9
                    }
                    Label {
                        id: newtext
                        text: modelData
                        color: tabbar.currentIndex == index ? "#f56565" : "black"
                        MouseArea {
                            enabled: index != tabbar.currentIndex
                            anchors.fill: parent
                            onClicked: {
                                if (index == 2) {
                                    stackView.searchMode = true
                                }
                                if (tabbar.currentIndex == 2) {
                                    stackView.searchMode = false
                                }
                                if (stackView.depth > 1) {
                                    stackView.pop()
                                }
                                if (index == 3) {
                                    if (!loginhandler.login) {
                                        stackView.tabBarNeeded = false
                                        stackView.push(loginpage)
                                        return;
                                    }

                                    stackView.push(submitpage)
                                }

                                tabbar.currentIndex = index
                                stackView.tabBarNeeded = true
                            }
                            cursorShape: Qt.PointingHandCursor
                        }
                    }
                }
            }
        }
        MyButton {
            visible: stackView.tabBarNeeded
            Layout.rightMargin: 70
            Layout.preferredWidth: 110
            Layout.preferredHeight: 50
            contentText.text: loginhandler.login ? "SignOut" : "Login / SignUp"
            contentText.color: "#f56565"
            bgitem.color: "transparent"
            bgitem.border.width: 1
            bgitem.border.color: "#f56565"
            onClicked: {
                if (loginhandler.login) {
                    loginhandler.signOut()
                    return
                }
                stackView.tabBarNeeded = false
                stackView.push(loginpage)
//                    tabbar.currentIndex = -1
            }
        }
    }
}
