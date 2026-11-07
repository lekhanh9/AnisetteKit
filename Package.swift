// swift-tools-version: 6.0
//
//  Package.swift
//  AnisetteKit
//
//  Created by Magesh K on 20/07/26.
//  Copyright © 2026 Magesh K. All rights reserved.
//

import PackageDescription

#if canImport(Darwin)
let unicornBinaryTargets: [Target] = [
    .binaryTarget(
        name: "Unicorn",
        url: "https://github.com/lekhanh9/unicorn-next/releases/download/2.1.4-local/Unicorn.xcframework.zip#AnisetteKit",
        checksum: "78a949606fa45df901020ef46a24c9bb548695fc0f7dffd52dcae6adc672e553"
    )
]
let unicornCoreDependencies: [Target.Dependency] = [
    "Unicorn"
]
let unicornLinkerSettings: [LinkerSetting] = []
#else
let unicornBinaryTargets: [Target] = []
let unicornCoreDependencies: [Target.Dependency] = []
let unicornLinkerSettings: [LinkerSetting] = [
    .linkedLibrary("unicorn")
]
#endif

let package = Package(
    name: "AnisetteKit",
    platforms: [
        .iOS(.v15),
        .macOS(.v12)
    ],
    products: [
        .library(
            name: "AnisetteKit",
            targets: ["AnisetteKit"]
        )
    ],
    dependencies: [],
    targets: [
        .target(
            name: "anisette_core",
            dependencies: unicornCoreDependencies,
            path: "Native",
            cSettings: [
                .headerSearchPath("."),
                .headerSearchPath("include"),
                .headerSearchPath("MacAuth")
            ],
            linkerSettings: unicornLinkerSettings
        ),
        .target(
            name: "AnisetteKit",
            dependencies: [
                "anisette_core"
            ],
            path: ".",
            exclude: [
                "Package.swift",
                "Native", 
                "Tests",
                "README.md",
                "LICENSE"
            ],
            sources: ["Sources"]
        ),
        .testTarget(
            name: "AnisetteKitTests",
            dependencies: [
                "AnisetteKit"
            ],
            path: "Tests"
        )
    ] + unicornBinaryTargets,
    cxxLanguageStandard: .cxx17
)
