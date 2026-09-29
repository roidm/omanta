#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QTest>

// Every hardcoded font size must scale with the desktop text factor.
// Rule: any `font.pixelSize:` in qml/ (except Fonts.qml itself) must go
// through Fonts.px(...) or through a scaled property (root.symbolSize,
// whose default is itself scaled). A bare number stays fixed while
// `gsettings get org.gnome.desktop.interface text-scaling-factor` grows,
// which is the bug this guards against.
class TestQmlFonts : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void everyPixelSizeUsesScaledHelper();
};

void TestQmlFonts::everyPixelSizeUsesScaledHelper()
{
    const QDir qmlDir(QStringLiteral(OMANTA_QML_DIR));
    const QStringList files = qmlDir.entryList({ QStringLiteral("*.qml") }, QDir::Files);
    QVERIFY2(!files.isEmpty(), "no QML found — check OMANTA_QML_DIR");

    QStringList offenders;
    for (const QString &name : files) {
        if (name == QLatin1String("Fonts.qml"))
            continue;
        QFile file(qmlDir.filePath(name));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const QStringList lines =
            QString::fromUtf8(file.readAll()).split(QLatin1Char('\n'));

        for (int i = 0; i < lines.size(); ++i) {
            const QString &line = lines.at(i);
            if (!line.contains(QLatin1String("font.pixelSize")))
                continue;
            // Allowed: scaled helper or scaled property indirection.
            if (line.contains(QLatin1String("Fonts.px("))
                || line.contains(QLatin1String("root.symbolSize")))
                continue;
            offenders << QStringLiteral("%1:%2: %3").arg(name).arg(i + 1).arg(line.trimmed());
        }
    }

    QVERIFY2(offenders.isEmpty(),
             qPrintable(QStringLiteral("font.pixelSize without Fonts.px() "
                                        "(use font.pixelSize: Fonts.px(N)): \n")
                        + offenders.join(QStringLiteral("\n"))));
}

QTEST_MAIN(TestQmlFonts)
#include "tst_qmlfonts.moc"
